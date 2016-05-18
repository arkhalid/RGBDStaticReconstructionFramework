#pragma once
#ifndef _KINECT_CAPTURE_APP
#define _KINECT_CAPTURE_APP

#include "KinectCapture.h"
#include <opencv2\opencv.hpp>
#include <vector>
class RGBDCaptureApp
{
public:
	RGBDCaptureApp();
	~RGBDCaptureApp();
	bool Run();
	
private:
	bool WriteData(std::string fileName);

	struct ImageData
	{
		cv::Mat colorIm;
		cv::Mat depthIm;
		LARGE_INTEGER timeStamp;
	};
	KinectCapture* m_kinCap;
	std::vector<ImageData> m_images;

};



#endif
