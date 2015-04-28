///////////////////////////////////////////////////////////////////////////
//
// Main class for HCI2 assignments
// Authors:
//		Stefan Neubert (2015)
//		Stephan Richter (2011)
//		Patrick Lühne (2012)
//
///////////////////////////////////////////////////////////////////////////

#include "Application.h"

#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "framework/DepthCamera.h"
#include "framework/KinectMotor.h"

int first_thresh = 3;
int second_thresh = 2;
int blur_size = 13;
int min_foot_area = 1400;

bool captured_reference = false;
int brighten_factor = 15;

static cv::Scalar ellipse_color = cv::Scalar(255,0,0);
static cv::Scalar line_color = cv::Scalar(0,255,0);

void Application::drawLines()
{
    cv::Point from;
    cv::Point to;
    
    for (int i = 0; i < contacts.size() - 1; i++) {
        from = contacts[i].center;
        to = contacts[i + 1].center;
        cv::line(m_bgrImage, from, to, line_color);
    }
}

void Application::addToContacts(cv::RotatedRect box)
{
    contacts.push_back(box);
}

bool Application::isFoot(std::vector<cv::Point> contour)
{
    if (cv::contourArea(contour) > min_foot_area) {
//        std::cout << cv::contourArea(cv::Mat(contour)) << std::endl;
        return true;
    } else {
        return false;
    }
    //    return cv::contourArea(cv::Mat(contour)) > min_foot_area;
}

void Application::drawEllipse(cv::RotatedRect box)
{
    cv::ellipse(m_bgrImage, box, ellipse_color, 3);
}


void Application::processFrame()
{	
    ///////////////////////////////////////////////////////////////////////////
    //
    // To do:
    //
    // This method will be called every frame of the camera. Insert code here in
    // order to fulfill the assignment. These images will help you doing so:
    //
    // * m_bgrImage: The image of the Kinect's color camera
    // * m_depthImage: The image of the Kinects's depth sensor
    // * m_outputImage: The image in which you can draw the touch circles.
    //
    ///////////////////////////////////////////////////////////////////////////
    
    // Sample code brightening up the depth image to make the values visible
    m_depthImage *= brighten_factor;
    
    // substract background
    cv::absdiff(m_reference, m_depthImage, m_depthImage);
    
    // converting to 8-Bit, also scale down values to 8-Bit scale
    cv::Mat m_depthImage_8CU1 = cv::Mat(480, 640, CV_8UC1);
    m_depthImage.convertTo(m_depthImage_8CU1, CV_8U, 1.0/256);
    
    
    // thresholding
    cv::threshold(m_depthImage_8CU1, m_outputImage, first_thresh, 255, cv::THRESH_TOZERO_INV);
    
    // blur image
    if (blur_size > 0) {
        cv::blur(m_outputImage, m_outputImage, cv::Size(blur_size, blur_size));
    }
    
    // make binary image
    cv::threshold(m_outputImage, m_outputImage, second_thresh, 255, cv::THRESH_BINARY);
    
    cv::Mat m_contour = cv::Mat(480, 640, CV_8UC1);
    m_outputImage.copyTo(m_contour);
    
    // find contures
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(m_contour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    // filter contures
//    contours.erase(std::remove_if(std::begin(contours), std::end(contours), isFoot), std::end(contours));
    
    // draw ellipses
    for (unsigned long i = 0; i < contours.size(); i++) {
        // less than 5 points throw an assertion error
        if (contours[i].size() >= 5) {
            cv::RotatedRect box = cv::fitEllipse(contours[i]);
            if (box.size.area() > 200) {
                drawEllipse(box);
                addToContacts(box);
            }
        }
    }
    if (contacts.size() >= 2) {
        drawLines();
    }
}

void Application::loop()
{
	int key = cv::waitKey(20);
	switch (key)
	{
	case 'q': // quit
		m_isFinished = true;
		break;
	case 's': // screenshot
		makeScreenshots();
		break;
	}

	m_depthCamera->getFrame(m_bgrImage, m_depthImage);
    
    if (!captured_reference) {
        m_depthImage.copyTo(m_reference);
        m_reference *= brighten_factor;
        captured_reference = true;
    }
    
	processFrame();

	cv::imshow("bgr", m_bgrImage);
	cv::imshow("depth", m_depthImage);
	cv::imshow("output", m_outputImage);
}

void Application::makeScreenshots()
{
	cv::imwrite("color.png", m_bgrImage);
	cv::imwrite("depth.png", m_depthImage);
	cv::imwrite("output.png", m_outputImage);
}

Application::Application()
	: m_isFinished(false)
	, m_depthCamera(nullptr)
	, m_kinectMotor(nullptr)
{
	// If you want to control the motor / LED
	// m_kinectMotor = new KinectMotor;

	m_depthCamera = new DepthCamera;

	// open windows
	cv::namedWindow("output", 1);
	cv::namedWindow("depth", 1);
	cv::namedWindow("bgr", 1);
    
    // add tracksbars
    cv::createTrackbar("blur_value", "output", &blur_size, 30);
    cv::createTrackbar("second_threshold_value", "output", &second_thresh, 256);
    cv::createTrackbar("first_threshold_value", "output", &first_thresh, 256);

    // create work buffer
	m_bgrImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_outputImage = cv::Mat(480, 640, CV_8UC1);
    
    m_reference = cv::Mat(480, 640, CV_16UC1);
}

Application::~Application()
{
	if (m_depthCamera) delete m_depthCamera;
	if (m_kinectMotor) delete m_kinectMotor;
}

bool Application::isFinished()
{
	return m_isFinished;
}
