#include"KinectCapture.h"

bool KinectCapture::InitCapture()
{
	INuiSensor * pNuiSensor;
	HRESULT hr;
	int iSensorCount = 0;
	hr = NuiGetSensorCount(&iSensorCount);
	if (FAILED(hr))
	{
		std::cout << "No ready Kinect found!" << std::endl;
		return false;
	}

	// Look at each Kinect sensor
	for (int i = 0; i < iSensorCount; ++i)
	{
		// Create the sensor so we can check status, if we can't create it, move on to the next
		hr = NuiCreateSensorByIndex(i, &pNuiSensor);
		if (FAILED(hr))
		{
			continue;
		}

		// Get the status of the sensor, and if connected, then we can initialize it
		hr = pNuiSensor->NuiStatus();
		if (S_OK == hr)
		{
			m_NuiSensor = pNuiSensor;
			break;
		}

		// This sensor wasn't OK, so release it since we're not using it
		pNuiSensor->Release();
	}

	if (nullptr != m_NuiSensor)
	{
		// Initialize the Kinect and specify that we'll be using depth
		hr = m_NuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
		if (SUCCEEDED(hr))
		{
			// Open a depth image stream to receive depth frames
			hr = m_NuiSensor->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_DEPTH,
				m_depthImageResolution,
				0,
				2,
				NULL,
				&m_DepthStreamHandle);
			if (SUCCEEDED(hr))
			{
				// Open a color image stream to receive color frames
				hr = m_NuiSensor->NuiImageStreamOpen(
					NUI_IMAGE_TYPE_COLOR,
					m_colorImageResolution,
					0,
					2,
					NULL,
					&m_ColorStreamHandle);
			}
			if (FAILED(hr))
			{
				std::cout << "Couldn't open color stream" << std::endl;
			}
		}
	}

	if (m_NearMode)
	{
		// Set near mode based on our internal state
		HRESULT nearHr = m_NuiSensor->NuiImageStreamSetImageFrameFlags(m_DepthStreamHandle, NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE);
	}
	if (nullptr == m_NuiSensor || FAILED(hr))
	{
		std::cout << "No ready Kinect found!" << std::endl;
		return false;
	}
	return (S_OK == hr);
}

bool KinectCapture::GetColorImage(cv::Mat& image)
{
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT lockedRect;
	HRESULT hr;
	// Get a color frame from Kinect
	image = cv::Mat::zeros(m_ColorHeight, m_ColorWidth,CV_8UC4);
	hr = m_NuiSensor->NuiImageStreamGetNextFrame(m_ColorStreamHandle, 100, &imageFrame);
	if (FAILED(hr))
	{
		if (hr == E_FAIL)
		{
			std::cout << "Kinect NuiImageStreamGetNextFrame call failed with unspecified"<< std::endl;
		}
		if (hr == E_INVALIDARG)
		{
			std::cout << "Kinect NuiImageStreamGetNextFrame call failed with dwFlag parameter is NULL" << std::endl;
		}
		if (hr == E_NUI_DEVICE_NOT_READY)
		{
			std::cout << "Kinect NuiImageStreamGetNextFrame call failed with kinect not initialized" << std::endl;
		}
		if (hr == E_OUTOFMEMORY)
		{
			std::cout << "Kinect NuiImageStreamGetNextFrame call failed with out of memory" << std::endl;
		}
		if (hr == E_POINTER)
		{
			std::cout << "Kinect NuiImageStreamGetNextFrame call failed with invalid handle" << std::endl;
		}
		if (hr == E_NUI_FRAME_NO_DATA)
		{
			return false;
		}
		return false;
	}
	INuiFrameTexture* colorTexture = imageFrame.pFrameTexture;
	// Lock the frame data to access depth data
	hr = colorTexture->LockRect(0, &lockedRect, NULL, 0);
	if (FAILED(hr) || lockedRect.Pitch == 0)
	{
		std::cout << "Error getting color texture pixels." << std::endl;
		return false;
	}
	// Copy the depth pixels so we can return the image frame
	errno_t err = memcpy_s(m_colorImagePixelBuffer, 4 * m_ColorImagePixels * sizeof(BYTE), lockedRect.pBits, colorTexture->BufferLen());
	colorTexture->UnlockRect(0);
	if (0 != err)
	{
		std::cout << "Error copying  color texture pixels." << std::endl;
		return false;
	}
	// Release the Kinect camera frame
	m_NuiSensor->NuiImageStreamReleaseFrame(m_ColorStreamHandle, &imageFrame);

	if (FAILED(hr))
	{
		std::cout << "Kinect Color stream NuiImageStreamReleaseFrame call failed." << std::endl;
		return false;
	}
	cv::Mat colorImg(m_ColorHeight, m_ColorWidth, CV_8UC4, m_colorImagePixelBuffer);
	image = colorImg;
	return true;
}

bool KinectCapture::GetDepthImage(cv::Mat & image)
{
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT lockedRect;
	HRESULT hr;
	image = cv::Mat::zeros(m_ColorHeight, m_ColorWidth, CV_16U);
	// Get a depth frame from Kinect
	hr = m_NuiSensor->NuiImageStreamGetNextFrame(m_DepthStreamHandle, 30, &imageFrame);
	if (FAILED(hr))
	{
		if (hr == E_NUI_FRAME_NO_DATA)
		{
			return false;
		}
		else
		{
			std::cout << "Kinect NuiImageStreamGetNextFrame call failed." << std::endl;
		}

		return false;
	}
	INuiFrameTexture* depthTexture = imageFrame.pFrameTexture;
	// Lock the frame data to access depth data
	hr = depthTexture->LockRect(0, &lockedRect, NULL, 0);
	if (FAILED(hr) || lockedRect.Pitch == 0)
	{
		std::cout << "Error getting depth texture pixels." << std::endl;
		return false;
	}
	// Copy the depth pixels so we can return the image frame
	errno_t err = memcpy_s(m_depthImagePixelBuffer, m_DepthImagePixels * sizeof(USHORT), lockedRect.pBits, depthTexture->BufferLen());
	depthTexture->UnlockRect(0);
	if (0 != err)
	{
		std::cout << "Error copying  depth texture pixels." << std::endl;
		return false;
	}

	// Release the Kinect camera frame
	m_NuiSensor->NuiImageStreamReleaseFrame(m_DepthStreamHandle, &imageFrame);

	if (FAILED(hr))
	{
		std::cout << "Kinect Depth stream NuiImageStreamReleaseFrame call failed." << std::endl;
		return false;
	}
	cv::Mat depthImg(m_DepthHeight,m_DepthWidth,CV_16U, m_depthImagePixelBuffer);
	image = depthImg;
	return true;
}

KinectCapture::KinectCapture():
m_depthImagePixelBuffer(nullptr),
m_colorImagePixelBuffer(nullptr),
m_alignedColorImagePixelBuffer(nullptr),
m_NuiSensor(nullptr),
m_colorImageResolution(NUI_IMAGE_RESOLUTION_640x480),
m_depthImageResolution(NUI_IMAGE_RESOLUTION_640x480)
{
	DWORD width, height;
	NuiImageResolutionToSize(m_depthImageResolution, width, height);
	m_DepthHeight = height;
	m_DepthWidth = width;
	m_DepthImagePixels = m_DepthHeight * m_DepthWidth;
	NuiImageResolutionToSize(m_colorImageResolution, width, height);
	m_ColorHeight = height;
	m_ColorWidth = width;
	m_ColorImagePixels = m_ColorHeight * m_ColorWidth;
	m_colorImagePixelBuffer = new BYTE[m_ColorHeight * m_ColorWidth * 4];
	m_alignedColorImagePixelBuffer = new BYTE[m_DepthHeight * m_DepthWidth * 4];
	m_depthImagePixelBuffer = new USHORT[m_DepthHeight * m_DepthWidth];
}


bool KinectCapture::GetAlignedColorImage(cv::Mat & image)
{
	if (m_alignedColorImagePixelBuffer == nullptr)
	{
		return false;
	}
	//Acquire unaligned color image
	if (!GetColorImage(image))
	{
		return false;
	}

	//Align Color Image

		for (int j = 0; j < m_DepthHeight; j++)
		{
			for (int i = 0; i < m_DepthWidth; i++) 
			{
				// Determine color pixel for depth pixel i,j
				long x;
				long y;
						NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
							m_colorImageResolution, // color frame resolution
							m_depthImageResolution, // depth frame resolution
							NULL,                         // pan/zoom of color image (IGNORE THIS)
							i, j, m_depthImagePixelBuffer[j*m_DepthWidth + i],                  // Column, row, and depth in depth image
							&x, &y        // Output: column and row (x,y) in the color image
							);
				// If out of bounds, then do not color this pixel
				if (x < 0 || y < 0 || x > m_ColorWidth || y > m_ColorHeight)
				{
					for (int ch = 0; ch < 3; ch++)
					{
						m_alignedColorImagePixelBuffer[4 * (j*m_DepthWidth + i) + ch] = 0;
					}
					m_alignedColorImagePixelBuffer[4 * (j*m_DepthWidth + i) + 3] = 255;
				}
				else {
					// Determine rgb color for depth pixel (i,j) from color pixel (x,y)
					for (int ch = 0; ch < 3; ch++)
					{
						m_alignedColorImagePixelBuffer[4 * (j*m_DepthWidth + i) + ch] = m_colorImagePixelBuffer[4 * (y*m_ColorWidth + x) + ch];
					}
					m_alignedColorImagePixelBuffer[4 * (j*m_DepthWidth + i) + 3] = 255;
				}
			}
		}
	
	cv::Mat colorImg(m_DepthHeight, m_DepthWidth, CV_8UC4, m_alignedColorImagePixelBuffer);
	image = colorImg;
	return true;
}


KinectCapture::~KinectCapture()
{
	// Clean up Kinect
	if (m_NuiSensor)
	{
		m_NuiSensor->NuiShutdown();
		m_NuiSensor->Release();
	}


	// clean up the depth pixel array
	if (m_depthImagePixelBuffer != nullptr) 
	{
		delete[] m_depthImagePixelBuffer;
	}
	
	if (m_colorImagePixelBuffer != nullptr)
	{
		delete[] m_colorImagePixelBuffer;
	}

	if (m_alignedColorImagePixelBuffer != nullptr)
	{
		delete[] m_alignedColorImagePixelBuffer;
	}
}