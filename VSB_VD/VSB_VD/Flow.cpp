#include "stdafx.h"
#include "Flow.h"

Flow::Flow(int W, int H)
{
	this->W = W;
	this->H = H;

	float scaleX = W / (float)flowW;
	float scaleY = H / (float)flowH;
	scale = cv::Vec2f(scaleX, scaleY);

	lines = cv::Mat(H, W, CV_32FC3);
	matCurl = cv::Mat(flowH, flowW, CV_32F);
	//matCurlColorized = cv::Mat(flowH, flowW, CV_32FC3);

	for (size_t i = 0; i < 20; i++)
	{
		points.push_back(cv::Vec2f((i + 5) * (flowW / 28), flowH * 0.8));
	}

	cv::namedWindow("flow");
	cv::setMouseCallback("flow", CallBackFunc, this);
}

Flow::~Flow()
{
}

void Flow::execute()
{
	std::vector<cv::Mat> flows;
	const int FLOWS_SIZE = 200;
	size_t i = 0;

	cv::Mat loadedMat;
	for (;;)
	{
		if (i < FLOWS_SIZE) {
			char fileName[100];
			snprintf(fileName, sizeof(fileName), "flow/u%05d.yml", i);
			cv::FileStorage storage(fileName, cv::FileStorage::Mode::FORMAT_AUTO | cv::FileStorage::Mode::READ);
			storage["flow"] >> loadedMat;
			storage.release();
			i++;
		}
		showFlowMat(loadedMat);

		//flows.push_back(loadedMat);

		int k = cv::waitKey(30);
		if (k == ' ') {
			std::cout << "paused " << std::endl;
			cv::waitKey(0);
			std::cout << "unpaused " << std::endl;
		}
		else if (k == 'q') {
			exit(0);
		}
		else if (k == 'r') {
			i = 0;
		}
		else if (k == 'd') {
			dt += 0.05;
			std::cout << "dt " << dt << std::endl;
		}
		else if (k == 'a') {
			dt -= 0.05;
			std::cout << "dt " << dt << std::endl;
		}
		else if (k == 'w') {
			precision += 5;
			std::cout << "precision " << precision << std::endl;
		}
		else if (k == 's') {
			precision -= 5;
			std::cout << "precision " << precision << std::endl;
		}
	}

	for (;;)
	{
		//showFlowMat(flows[flows.size() - 1]);
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
	cv::minMaxLoc(output, &min, &max); // std::cout << " output min: " << min << ", max: " << max << std::endl;
	max = MAX(abs(min), abs(max));
	min = -max;
	//std::cout << " output min: " << min << ", max: " << max << std::endl;

	cv::Mat outputNorm = cv::Mat(output.rows, output.cols, CV_32FC1);

	cv::normalize(output, outputNorm, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	outputNorm.convertTo(outputNorm, CV_8UC1);
	//cv::normalize(output, outputNorm, 0, 1, cv::NORM_MINMAX, CV_32FC1);
	applyColorMap(outputNorm, outputColor, cv::COLORMAP_JET);

	//std::cout << "-----------" << std::endl;
	//cv::minMaxLoc(input, &min, &max); std::cout << " input min: " << min << ", max: " << max << std::endl;
	//cv::minMaxLoc(outputNorm, &min, &max); std::cout << " outputNorm min: " << min << ", max: " << max << std::endl;
	//cv::minMaxLoc(outputColor, &min, &max); std::cout << " outputColor min: " << min << ", max: " << max << std::endl;
}

cv::Vec2f getFlowSubpix(const cv::Mat& flow, cv::Point2f pt)
{
	cv::Mat patch;
	cv::remap(flow, patch, cv::Mat(1, 1, CV_32FC2, &pt), cv::noArray(),
		cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
	return patch.at<cv::Vec2f>(0, 0);
}

cv::Vec2f Flow::RungeKutta(cv::Mat& flow, cv::Vec2f& p, float dt) 
{
	cv::Vec2f K1 = getFlowSubpix(flow, p) * dt;
	cv::Vec2f K2 = getFlowSubpix(flow, p + K1 * 0.5f) * dt;
	cv::Vec2f K3 = getFlowSubpix(flow, p + K2 * 0.5f) * dt;
	cv::Vec2f K4 = getFlowSubpix(flow, p + K3) * dt;

	cv::Vec2f mp = p + ((1. / 6) * (K1 + 2*K2 + 2*K3 + K4));
	return mp;
}

cv::Vec2f Flow::movePoint(cv::Mat& flow, cv::Vec2f& p, float dt) 
{
	return RungeKutta(flow, p, dt);
}

std::vector<cv::Vec2f> Flow::movePoints(cv::Mat& flow, std::vector<cv::Vec2f> points, float dt)
{
	std::vector<cv::Vec2f> nPoints;
	for (size_t i = 0; i < points.size(); i++)
	{
		nPoints.push_back(movePoint(flow, points[i], dt));
	}
	return nPoints;
}

void Flow::showFlowMat(cv::Mat & mat)
{
	matFlow = cv::Mat(H, W, CV_32FC3);

	double min, max;
	cv::minMaxLoc(mat, &min, &max);
	float arrowScale = MAX(abs(min), abs(max)) / MIN(scale.x, scale.y);

	curl(mat, matCurl, matCurlColorized);
	
	for (int y = 0; y < flowH; y++)
	{
		for (int x = 0; x < flowW; x++)
		{
			auto v = mat.at<cv::Point2f>(y, x) * arrowScale;
			cv::Point2f pos((x + 0.5f) * scale.x, (y + 0.5f) * scale.y);
			auto start = pos - v;
			auto end = pos + v;
			if (x > 0 && y > 0 && x < 127 && y < 127) {
				auto color = matCurlColorized.at<cv::Vec3b>(y-1, x-1);
				//cv::line(matFlow, start, end, cv::Scalar(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0), 1);
				cv::arrowedLine(matFlow, start, end, cv::Scalar(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0), 1, cv::LINE_AA, 0, 0.3);
			}
		}
	}

	for (size_t i = 0; i < precision; i++)
	{
		auto points2 = movePoints(mat, points, dt);

		for (size_t i = 0; i < points.size(); i++)
		{
			auto point = points2[i].mul(scale);
			auto point_old = points[i].mul(scale);
			//cv::circle(matFlow, cv::Point(point[0], point[1]), 5, cv::Scalar(255, 0, 0), -1);
			cv::line(lines, cv::Point(point_old[0], point_old[1]), cv::Point(point[0], point[1]), cv::Scalar(0, 0, 255));
		}

		points = points2;
	}
	for (size_t i = 0; i < points.size(); i++)
	{
		auto point = points[i].mul(scale);
		cv::circle(matFlow, cv::Point(point[0], point[1]), 5, cv::Scalar(0, 0, 255), -1);
	}

	cv::add(matFlow, lines, matFlow);

	cv::imshow("flow", matFlow);
}
