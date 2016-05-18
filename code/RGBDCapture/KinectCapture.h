#pragma once
#ifndef _KINECT_CAPTURE
#define _KINECT_CAPTURE
#include "RGBDCapture.h"
#include <windows.h>

#include <NuiApi.h>
#include <opencv2\opencv.hpp>
class KinectCapture: public RGBDCapture
{
public:
	KinectCapture();
	~KinectCapture();
	virtual bool InitCapture();
	virtual bool GetColorImage(cv::Mat& image);
	virtual bool GetDepthImage(cv::Mat& image);
	virtual bool GetAlignedColorImage(cv::Mat& image);
private:
	//Kinect sensor variables
	INuiSensor*					m_NuiSensor; //sensor handle
	//Kinect Image resolution Variables
	NUI_IMAGE_RESOLUTION        m_depthImageResolution; 
	NUI_IMAGE_RESOLUTION		m_colorImageResolution; 
	int                         m_DepthWidth; //width of depth image
	int                         m_DepthHeight;	//height of depth image
	int                         m_DepthImagePixels; // number of pixels in depth image
	int                         m_ColorWidth;
	int                         m_ColorHeight;
	int                         m_ColorImagePixels;
	//Handles to streams
	HANDLE                      m_DepthStreamHandle;
	HANDLE                      m_ColorStreamHandle;
	//
	bool m_NearMode;

	//
	USHORT*						m_depthImagePixelBuffer;
	BYTE*						m_colorImagePixelBuffer;
	BYTE*						m_alignedColorImagePixelBuffer;
};
#endif

