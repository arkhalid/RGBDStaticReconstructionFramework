#pragma once
#ifndef _RGBD_CAPTURE
#define _RGBD_CAPTURE
#include<opencv2\opencv.hpp>

class RGBDCapture
{
public:
	struct CamProperties
	{
		int width;
		int height;
	};


	RGBDCapture() {}
	RGBDCapture(CamProperties camProperties) {}
	~RGBDCapture() {}
	virtual bool InitCapture() = 0;
	virtual bool GetColorImage(cv::Mat& image) = 0;
	virtual bool GetDepthImage(cv::Mat& image) = 0;
	virtual bool GetAlignedColorImage(cv::Mat& image) = 0;
private:
	CamProperties m_camProperties;
};

#endif

