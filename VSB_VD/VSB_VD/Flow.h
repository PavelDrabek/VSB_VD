#pragma once
class Flow
{
public:
	Flow(int W, int H);
	~Flow();

	const int flowW = 128, flowH = 128;
	
	cv::Mat matFlow;
	cv::Mat matCurl, lines;
	cv::Mat matCurlColorized;
	int W, H;

	void execute();
	void curl(cv::Mat & input, cv::Mat & output, cv::Mat & outputColor);
	cv::Vec2f RungeKutta(cv::Mat & flow, cv::Vec2f & p, float dt);
	cv::Vec2f movePoint(cv::Mat & flow, cv::Vec2f & p, float dt);
	std::vector<cv::Vec2f> movePoints(cv::Mat & flow, std::vector<cv::Vec2f> points, float dt);
	void colorizeCurl(cv::Mat & input, cv::Mat & output);
	void showFlowMat(cv::Mat & mat);

	static void CallBackFunc(int event, int x, int y, int flags, void* userdata)
	{
		Flow* f = (Flow*)userdata;
		if (event == cv::EVENT_LBUTTONDOWN)
		{
			f->points.push_back(cv::Vec2f(x / f->scale.x, y / f->scale.y));
		}
	}

private:
	std::vector<cv::Vec2f> points;
	cv::Point2f scale;
	float dt = 0.02;
	int precision = 50;
};

