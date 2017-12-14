// VDDepth.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <opencv2\opencv.hpp>

cv::Mat LoadData()
{
	// f = 570.342

	//char fileName[100];
	//snprintf(fileName, sizeof(fileName), "flow/u%05d.yml", 0);
	cv::FileStorage storage("data/000.gz", cv::FileStorage::Mode::FORMAT_AUTO | cv::FileStorage::Mode::READ);
	cv::Mat loadedMat;
	storage["depth"] >> loadedMat;

	loadedMat.convertTo(loadedMat, CV_32FC1);

	//loadedMat *= 5;
	return loadedMat;
}

cv::Mat LoadSynthData()
{
	cv::FileStorage storage("data_synthetic/scene_eda_2.yml", cv::FileStorage::Mode::FORMAT_AUTO | cv::FileStorage::Mode::READ);
	cv::Mat loadedMat;
	storage["depth"] >> loadedMat;
	loadedMat.convertTo(loadedMat, CV_32FC1);

	//loadedMat *= .001;
	return loadedMat;
}

cv::Mat LoadSynthNormals()
{
	cv::FileStorage storage("data_synthetic/scene_eda_2.yml", cv::FileStorage::Mode::FORMAT_AUTO | cv::FileStorage::Mode::READ);
	cv::Mat loadedMat;
	storage["normals"] >> loadedMat;

	return loadedMat;
}

inline bool isInside(cv::Mat& mat, int x, int y)
{
	return (x >= 0 && y >= 0 && x < mat.cols && y < mat.rows);
}

inline cv::Vec3f depthToMeters(int x, int y, int w, int h, float D, float f_pk)
{
	return cv::Vec3f(
		(x - w / 2) * D / f_pk,
		-(-y + h / 2) * D / f_pk,
		D
	);

	//return cv::Vec3f(
	//	(x - w / 2) * (f_pk), 
	//	(-y + h / 2) * (f_pk),
	//	D
	//);
}

std::vector<cv::Point> createOffsets(int rx, int ry)
{
	std::vector<cv::Point> offsets;
	for (int x = -rx; x <= rx; x++) {
		for (int y = -ry; y <= ry; y++) {
			offsets.push_back(cv::Point(x, y));
		}
	}
	return offsets;
}

void depthToMeters(cv::Mat& depth, cv::Mat& output, float f_pk)
{
	int w = depth.cols;
	int h = depth.rows;

	for (size_t y = 0; y < h; y++)
	{
		for (size_t x = 0; x < w; x++)
		{
			float D = depth.at<float>(y, x);
			if (D <= 0) {
				output.at<cv::Vec3f>(y, x) = cv::Vec3f(0,0,0);
			} else {
				cv::Vec3f v = depthToMeters(x, y, w, h, D, f_pk);
				output.at<cv::Vec3f>(y, x) = v;
			}
		}
	}
}

std::vector<cv::Vec3f> getNbrValues(cv::Mat& meters, int x, int y, std::vector<cv::Point>& nbr)
{
	std::vector<cv::Vec3f> values;

	for (size_t i = 0; i < nbr.size(); i++)
	{
		int _x = x + nbr[i].x;
		int _y = y + nbr[i].y;

		if (!isInside(meters, _x, _y)) {
			continue;
		}

		auto v = meters.at<cv::Vec3f>(_y, _x);

		if (v[2] > 0) {
			values.push_back(v);
		}
	}

	return values;
}

cv::Vec3f getAvg(std::vector<cv::Vec3f>& values)
{
	cv::Vec3f sum = 0;
	for (size_t i = 0; i < values.size(); i++) {
		sum += values[i];
	}

	return sum / (int)values.size();
}

cv::Vec3f getAvg(cv::Mat& meters, int x, int y, std::vector<cv::Point>& nbr)
{
	int count = 0;
	cv::Vec3f sum = 0;
	for (size_t i = 0; i < nbr.size(); i++)
	{
		int _x = x + nbr[i].x;
		int _y = y + nbr[i].y;

		if (!isInside(meters, _x, _y)) {
			continue;
		}

		auto v = meters.at<cv::Vec3f>(y, x);
		if (v[0] <= 0) {
			continue;
		}

		count++;
		sum += v;
	}

	return sum * (1.0f / count);
}

cv::Mat getEpsilon(cv::Mat& meters, int x, int y, std::vector<cv::Vec3f>& nbrValues) 
{
	cv::Vec3f avg = getAvg(nbrValues);
	cv::Mat epsilon = cv::Mat::zeros(3, 3, CV_32FC1);

	for (size_t i = 0; i < nbrValues.size(); i++)
	{
		epsilon.at<float>(0, 0) += (nbrValues[i][0] - avg[0]) * (nbrValues[i][0] - avg[0]); // E[(xi - x_) * (xi - x_)]
		epsilon.at<float>(0, 1) += (nbrValues[i][0] - avg[0]) * (nbrValues[i][1] - avg[1]); // E[(xi - x_) * (yi - y_)]
		epsilon.at<float>(0, 2) += (nbrValues[i][0] - avg[0]) * (nbrValues[i][2] - avg[2]); // E[(xi - x_) * (zi - z_)]

		epsilon.at<float>(1, 0) += (nbrValues[i][1] - avg[1]) * (nbrValues[i][0] - avg[0]); // E[(yi - y_) * (xi - x_)]
		epsilon.at<float>(1, 1) += (nbrValues[i][1] - avg[1]) * (nbrValues[i][1] - avg[1]); // E[(yi - y_) * (yi - y_)]
		epsilon.at<float>(1, 2) += (nbrValues[i][1] - avg[1]) * (nbrValues[i][2] - avg[2]); // E[(yi - y_) * (zi - z_)]

		epsilon.at<float>(2, 0) += (nbrValues[i][2] - avg[2]) * (nbrValues[i][0] - avg[0]); // E[(zi - z_) * (xi - x_)]
		epsilon.at<float>(2, 1) += (nbrValues[i][2] - avg[2]) * (nbrValues[i][1] - avg[1]); // E[(zi - z_) * (yi - y_)]
		epsilon.at<float>(2, 2) += (nbrValues[i][2] - avg[2]) * (nbrValues[i][2] - avg[2]); // E[(zi - z_) * (zi - z_)]
	}
	epsilon *= (1.0f / nbrValues.size());
	
	if (y == 200)
	{
		//std::cout << nbrValues.size() << std::endl;
		//std::cout << epsilon << std::endl;
	}

	return epsilon;
}

cv::Vec3f calcNormal(cv::Mat& meters, int x, int y, std::vector<cv::Point>& nbr)
{
	auto nbrValues = getNbrValues(meters, x, y, nbr);
	if (nbrValues.size() < 3) {
		return cv::Vec3f(0,0,0);
	}

	cv::Mat epsilon = getEpsilon(meters, x, y, nbrValues);

	cv::Mat values, vectors;
	cv::eigen(epsilon, values, vectors);

	cv::Vec3f n = vectors.row(2);
	n = cv::normalize(n);

	if (n.dot(cv::Vec3f(0, 0, 1)) < 0) {
		n = -n;
	}

	return n;
}

cv::Mat calcNormals(cv::Mat& meters) {
	cv::Mat normals = cv::Mat(meters.rows, meters.cols, CV_32FC3);
	std::vector<cv::Point> offsets = createOffsets(2, 2);

	for (size_t x = 0; x < meters.cols; x++) {
		for (size_t y = 0; y < meters.rows; y++) {
			normals.at<cv::Vec3f>(y, x) = calcNormal(meters, x, y, offsets);
		}
	}

	return normals;
}

void x(cv::Mat depth) 
{
	cv::Mat meters = cv::Mat(depth.rows, depth.cols, CV_32FC3);
	depthToMeters(depth, meters, 570.342f);
	cv::Mat normals = calcNormals(meters);

	cv::Mat normalsSynth = LoadSynthNormals();

	//cv::imshow("depth", depth * 10);
	//cv::imshow("meters", meters * 100000);
	cv::imshow("normals", normals);
	cv::imshow("normalsSynth", normalsSynth);
	//cv::imshow("meters", meters * 0.001f);
	cv::waitKey(0);
}

int main()
{
	cv::Mat data = LoadData();
	//cv::Mat data = LoadSynthData();

	x(data);

    return 0;
}

