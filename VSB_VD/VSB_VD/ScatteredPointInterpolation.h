#pragma once
	
class ScatteredPointInterpolation
{
public:
	ScatteredPointInterpolation();
	~ScatteredPointInterpolation();

	const int N = 300; // number of samples
	const float x0 = -3.0; // sampling domain ranges
	const float y0 = -3.0;
	const float x1 = 3.0;
	const float y1 = 3.0;

	const int gd = 2; // dataset geometrical dimension


	double Gaussian2D(const double A, const double x, const double y, const double x0, const double y0, const double sx, const double sy);
	double Gaussian2DIntegral(const double A, const double sx, const double sy);
	void DrawSample(cv::Mat & mat, cv::Mat & points, cv::Mat & values);
	void Execute();
};

