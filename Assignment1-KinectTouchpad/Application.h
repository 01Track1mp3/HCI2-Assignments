#pragma once

#include <opencv2/core/core.hpp>

class DepthCamera;
class KinectMotor;

class Application
{
public:
	Application();
	virtual ~Application();

	void loop();

	void processFrame();
    static bool isFoot(std::vector<cv::Point> contour);
    
    void addToContacts(cv::RotatedRect);
    void drawLines();
    void drawLastLine();
    void drawEllipse(cv::RotatedRect);
    void handleNoContact();
    
	void makeScreenshots();
	void clearOutputImage();
    
    int recognizeDigit();

	bool isFinished();


protected:
	DepthCamera *m_depthCamera;
	KinectMotor *m_kinectMotor;

	cv::Mat m_bgrImage;
	cv::Mat m_depthImage;
	cv::Mat m_outputImage;
    
    cv::Mat m_reference;
    
    std::vector<std::vector<cv::Point> > contacts;
    
    bool set_reference_image = false;
	bool m_isFinished;
};