#include "RGBDCaptureApp.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
RGBDCaptureApp* mainApp;
bool RGBDCaptureApp::Run()
{
	cv::Mat depthIm, colorIm, colorImAligned;
	bool hasDepth;
	bool isRecording = false;
	//Initialize Kinect
	if(!m_kinCap->InitCapture())
	{
		std::cout << "Couldn't initialize kinect" << std::endl;
	}
	//Loop to acquire images
	while (true)
	{
		hasDepth = false;
		if (m_kinCap->GetDepthImage(depthIm))
		{
			if (isRecording)
			{
				cv::putText(depthIm, "Recording", cv::Point2d(100.0,100.0), cv::FONT_HERSHEY_PLAIN, 4.0, cv::Scalar(255, 0, 255.0));
			}
			imshow("Depth", depthIm);
			hasDepth = true;
		}
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		if (m_kinCap->GetAlignedColorImage(colorImAligned))
		{
			if (hasDepth && isRecording)
			{
				ImageData newData;
				newData.colorIm = colorImAligned.clone();
				newData.depthIm = depthIm.clone();
				newData.timeStamp = currTime;
				m_images.push_back(newData);
			}
			if (isRecording)
			{
				cv::putText(colorImAligned, "Recording", cv::Point2d(100.0, 100.0), cv::FONT_HERSHEY_PLAIN, 4.0, cv::Scalar(255, 0, 255.0));
			}

			imshow("Color Aligned", colorImAligned);
		}
		int k = cvWaitKey(1);
		if (k == 's')
		{
			isRecording = !isRecording;
		}
		if (k == 'q')
		{
			break;
		}
	}
	//if (!WriteData("F:/arkhalid/Google Drive/ToMapWork/Courses/4.5th Semester/RGBDStaticReconstructionFramework/code/Data/"))
	if (!WriteData("./Data/"))
	{
		std::cout << "Error saving images" << std::endl;
	}
	return true;
}

bool RGBDCaptureApp::WriteData(std::string folderName)
{
	cv::Mat tryImg = cv::Mat::zeros(500, 500, CV_8UC4);
	std::cout << "Saving " << m_images.size() << "images..."<<std::endl;
	std::stringstream filenameStream;
	for (int i = 0; i < m_images.size(); i++)
	{
		std::string filename;
		filenameStream<<folderName<< "rgb_" << std::setfill('0')<< std::setw(5)<< i << ".png";
		if (!cv::imwrite(filenameStream.str(), m_images[i].colorIm))
		{
			return false;
		}
		filenameStream.clear();
		filenameStream.str("");
		filenameStream << folderName << "depth_" << std::setfill('0') << std::setw(5) << i << ".png";
		if (!cv::imwrite(filenameStream.str(), m_images[i].depthIm))
		{
			return false;
		}
		filenameStream.clear();
		filenameStream.str("");
	}
	//Write times to file
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	filenameStream << folderName << "times.txt";
	std::ofstream fout;
	fout.open(filenameStream.str());
	filenameStream.str("");
	if (fout.is_open())
	{
		for (int i = 0; i < m_images.size(); i++)
		{
			double time = static_cast<double>(m_images[i].timeStamp.QuadPart) / static_cast<double>(frequency.QuadPart);
			fout << std::setfill('0') << std::setw(5) << i << "\t" << std::fixed<<std::setprecision(10) <<time << std::endl;
		}
	}
	else
	{
		return false;
	}
	fout.close();
	std::cout << "Done"<< std::endl;
	return true;
}

RGBDCaptureApp::RGBDCaptureApp()
{
	m_kinCap = new KinectCapture();
}


RGBDCaptureApp::~RGBDCaptureApp()
{
	if (m_kinCap != nullptr)
	{
		delete m_kinCap;
	}
}



int main()
{
	mainApp = new RGBDCaptureApp();
	mainApp->Run();
	int x;
	std::cin >> x;
	return 0;
}