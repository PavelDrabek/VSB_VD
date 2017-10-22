#pragma once
class Flow
{
public:
	Flow(int W, int H);
	~Flow();

	const int flowW = 128, flowH = 128;
	
	cv::Mat matFlow;
	cv::Mat matCurl;
	cv::Mat matCurlColorized;
	int W, H;

	void execute();
	void curl(cv::Mat & input, cv::Mat & output, cv::Mat & outputColor);
	void colorizeCurl(cv::Mat & input, cv::Mat & output);
	void showFlowMat(cv::Mat & mat);
};

