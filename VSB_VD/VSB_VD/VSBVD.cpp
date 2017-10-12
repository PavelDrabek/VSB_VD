// VSBVD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>

#define M_PI 3.14

inline double SQR(double x)
{
	return x*x;
}

inline double Gaussian2D(const double A = 1,  const double x = 1, const double y = 1, const double x0 = 0, const double y0 = 0, const double sx = 1, const double sy = 1)
{
	return A * exp(-( SQR(x - x0) / (2 * SQR(sx)) + SQR(y - y0) / (2 * SQR(sy) )));
}

double Gaussian2DIntegral(const double A = 1, const double sx = 1, const double sy = 1)
{
	return 2.0 * M_PI * A * abs(sx) * abs(sy);
}

void DrawSample(cv::Mat& mat, cv::Mat& points, cv::Mat& values) 
{
	double scaleX = mat.cols / 6.0;
	double scaleY = mat.rows / 6.0;
	for (int i = 0; i < points.rows; i++)
	{
		float x = points.at<float>(i, 0) * scaleX + mat.cols / 2;
		float y = points.at<float>(i, 1) * scaleY + mat.rows / 2;
		float v = values.at<float>(i);
		cv::circle(mat, cv::Point(x, y), 3, cv::Scalar(1, 0, v), -1);

		printf("%f \n", v);
	}
}

float random() {
	return (rand() % 10000) / 10000.0f;
}

void ScatteredPointInterpolation() {

	srand(time(NULL));

	const int N = 300; // number of samples
	const float x0 = -3.0; // sampling domain ranges
	const float y0 = -3.0;
	const float x1 = 3.0;
	const float y1 = 3.0;

	const int gd = 2; // dataset geometrical dimension
	cv::Mat sample_points = cv::Mat(N, gd, CV_32F);
	cv::Mat sample_values = cv::Mat(N, 1, CV_32F);

	// fill in sample points and sample values, i.e. {p_i, f_i}
	for (int i = 0; i < N; i++)
	{
		float x = (random() * (x1 - x0)) + x0;
		float y = (random() * (y1 - y0)) + y0;
		sample_points.at<float>(i, 0) = x;
		sample_points.at<float>(i, 1) = y;
		sample_values.at<float>(i) = Gaussian2D(1, x, y, 0, 0);
	}

	cv::Mat samples = cv::Mat(400, 400, CV_32FC3);
	DrawSample(samples, sample_points, sample_values);
	cv::imshow("samples", samples);

	// k-NN structures
	cv::flann::KDTreeIndexParams index = cv::flann::KDTreeIndexParams();
	cv::flann::Index nn = cv::flann::Index(sample_points, index);
	cv::flann::SearchParams search_params = cv::flann::SearchParams(256);

	// reconstruction

	cv::Mat query = cv::Mat(1, gd, CV_32F);
	cv::Mat indices = cv::Mat(N, 1, CV_32S);
	cv::Mat dists = cv::Mat(N, 1, CV_32F);

	cv::Mat reconstruction = cv::Mat(400, 400, CV_32FC3);
	cv::Mat rec_nearest = cv::Mat(400, 400, CV_32FC3);
	double radius = 5;

	double scaleX = reconstruction.cols / 6.0;
	double scaleY = reconstruction.rows / 6.0;
	// for each element of reconstructed domain do
	for (int x = 0; x < reconstruction.cols; x++)
	{
		for (int y = 0; y < reconstruction.rows; y++) {
			float xx = (x / (double)reconstruction.cols) * (x1 - x0) + x0;
			float yy = (y / (double)reconstruction.rows) * (y1 - y0) + y0;
			query.at<float>(0) = xx;
			query.at<float>(1) = yy;

			//nn.knnSearch(query, indices, dists, 10, search_params);
			const int no_indices = nn.radiusSearch(query, indices, dists, radius, N, search_params);

			float vn = sample_values.at<float>(indices.at<int>(0));
			float sw = 0; // sum of weights
			float sv = 0; // sum of values
			for (int i = 0; i < no_indices; ++i)
			{
				float d = dists.at<float>(i);
				float w = SQR( MAX(0, radius - d) / (radius * d) );
				float v = sample_values.at<float>(indices.at<int>(i));
				sv += v * w;
				sw += w;
			}

			// store result of interpolation 
			rec_nearest.at<cv::Vec3f>(y, x) = cv::Vec3f(1, 0, vn);
			reconstruction.at<cv::Vec3f>(y, x) = cv::Vec3f(1, 0, sv/sw);
		}
	}

	cv::imshow("reconst nearest", rec_nearest);
	cv::imshow("reconstruction", reconstruction);
	cv::waitKey(30);

	getchar();
}

int main()
{
	ScatteredPointInterpolation();
    return 0;
}

