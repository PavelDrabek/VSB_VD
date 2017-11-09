#include "stdafx.h"
#include "Flow.h"

Flow::Flow(int W, int H)
{
	this->W = W;
	this->H = H;

	matCurl = cv::Mat(flowH, flowW, CV_32F);
	//matCurlColorized = cv::Mat(flowH, flowW, CV_32FC3);
}

Flow::~Flow()
{
}

void Flow::execute()
{
	std::vector<cv::Mat> flows;
	const int FLOWS_SIZE = 200;

	for (size_t i = 0; i < FLOWS_SIZE; i++)
	{
		char fileName[100];
		snprintf(fileName, sizeof(fileName), "flow/u%05d.yml", i);
		cv::FileStorage storage(fileName, cv::FileStorage::Mode::FORMAT_AUTO | cv::FileStorage::Mode::READ);
		cv::Mat loadedMat;
		storage["flow"] >> loadedMat;

		showFlowMat(loadedMat);

		flows.push_back(loadedMat);
		storage.release();
	}


	cv::waitKey();
}

void Flow::curl(cv::Mat& input, cv::Mat& output, cv::Mat& outputColor) 
{
	for (int y = 0; y < flowH; y++)
	{
		for (int x = 0; x < flowW; x++)
		{
			if (x > 0 && y > 0 && x < flowW - 1 && y < flowH-1) {

				auto v = input.at<cv::Point2f>(y, x);
				auto dy = (input.at<cv::Point2f>(y - 1, x).x - input.at<cv::Point2f>(y + 1, x).x);
				auto dx = (input.at<cv::Point2f>(y, x - 1).y - input.at<cv::Point2f>(y, x + 1).y);
				output.at<float>(y, x) = dx - dy;
			}
			else {
				output.at<float>(y, x) = 0;
			}
		}
	}

	double min, max;
	cv::minMaxLoc(output, &min, &max); std::cout << " output min: " << min << ", max: " << max << std::endl;
	max = MAX(abs(min), abs(max));
	min = -max;
	std::cout << " output min: " << min << ", max: " << max << std::endl;

	cv::Mat outputNorm = cv::Mat(output.rows, output.cols, CV_32FC1);

	cv::normalize(output, outputNorm, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	outputNorm.convertTo(outputNorm, CV_8UC1);
	//cv::normalize(output, outputNorm, 0, 1, cv::NORM_MINMAX, CV_32FC1);
	applyColorMap(outputNorm, outputColor, cv::COLORMAP_COOL);

	std::cout << "-----------" << std::endl;
	cv::minMaxLoc(input, &min, &max); std::cout << " input min: " << min << ", max: " << max << std::endl;
	cv::minMaxLoc(outputNorm, &min, &max); std::cout << " outputNorm min: " << min << ", max: " << max << std::endl;
	cv::minMaxLoc(outputColor, &min, &max); std::cout << " outputColor min: " << min << ", max: " << max << std::endl;

}

void Flow::showFlowMat(cv::Mat & mat)
{
	matFlow = cv::Mat(H, W, CV_32FC3);

	float scaleX = W / (float)flowW;
	float scaleY = H / (float)flowH;
	double min, max;
	cv::minMaxLoc(mat, &min, &max);
	float arrowScale = MAX(abs(min), abs(max)) / MIN(scaleX, scaleY);

	curl(mat, matCurl, matCurlColorized);
	
	for (int y = 0; y < flowH; y++)
	{
		for (int x = 0; x < flowW; x++)
		{
			auto v = mat.at<cv::Point2f>(y, x) * arrowScale;
			cv::Point2f pos((x + 0.5f) * scaleX, (y + 0.5f) * scaleY);
			auto start = pos - v;
			auto end = pos + v;
			if (x > 0 && y > 0 && x < 127 && y < 127) {
				auto color = matCurlColorized.at<cv::Vec3b>(y-1, x-1);
				cv::line(matFlow, start, end, cv::Scalar(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0), 2 );
			}
		}
	}

	cv::imshow("flow", matFlow);
	cv::waitKey(10);
}
