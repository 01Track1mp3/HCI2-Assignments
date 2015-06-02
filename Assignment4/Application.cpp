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
#include "framework/SkeletonTracker.h"

#include "Calibration.h"

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <boost/thread.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include "uist-game/GameServer.h"
#include "uist-game/GameClient.h"
#include "uist-game/Game.h"
#include "uist-game/GameUnit.h"

const int Application::uist_level = 1;
const char* Application::uist_server = "127.0.0.1";

using namespace cv;
using namespace std;

bool captured_reference = false;
int brighten_factor = 15;

Point lastTouch;
bool hasLastTouch = false;

void Application::warpCameraToUntransformed() {
    Mat homography = m_calibration->cameraToPhysical();// * m_calibration->physicalToProjector();
    warpPerspective(m_depthImage, m_depthImageUntransformed, homography, Size(640, 480), INTER_NEAREST);

    m_depthImage = Mat(480, 640, CV_16UC1);
    m_depthImage = m_depthImageUntransformed;
}

void Application::warpUntransformedToTransformed()
{
	///////////////////////////////////////////////////////////////////////////
	//
	// To do:
	//
	// In this method, you have to warp the image in order to project it on
	// the floor so that it appears undistorted
	//
	// * m_outputImage: The image you have to distort in a way that it appears
	//                  undistorted on the floor
	// * m_calibration: The calibration class giving you access to the matrices
	//                  you have computed
	//
	///////////////////////////////////////////////////////////////////////////
    Mat homography = m_calibration->projectorToPhysical(); // * m_calibration->physicalToCamera();
    warpPerspective(m_gameImage, m_outputImage, homography, Size(640, 480), INTER_NEAREST);
}

bool Application::isFoot(std::vector<cv::Point> contour)
{
    return cv::contourArea(contour) > 100;
}

static cv::Scalar ellipse_color = cv::Scalar(255,0,0);
void Application::drawEllipse(cv::RotatedRect box)
{
    cv::ellipse(m_bgrImage, box, ellipse_color, 3);
}

void Application::processTouch()
{
    ///////////////////////////////////////////////////////////////////////////
    //
    // * m_bgrImage: The image of the Kinect's color camera
    // * m_depthImage: The image of the Kinects's depth sensor
    // * m_outputImage: The image in which you can draw the touch circles.
    //
    ///////////////////////////////////////////////////////////////////////////
    
    // brightening up the depth image to make the values visible
    m_depthImage *= brighten_factor;
    
    // substract background
    cv::absdiff(m_reference, m_depthImage, m_depthImage);
    
    // converting to 8-Bit, also scale down values to 8-Bit scale
    cv::Mat m_depthImage_8CU1 = cv::Mat(480, 640, CV_8UC1);
    m_depthImage.convertTo(m_depthImage_8CU1, CV_8U, 1.0/256);
    
    // thresholding
    cv::threshold(m_depthImage_8CU1, m_outputImage, 4, 255, cv::THRESH_TOZERO_INV);
    
    // blur image
    cv::blur(m_outputImage, m_outputImage, cv::Size(13, 13));
    
    cv::Mat m_outputImage_thresh = cv::Mat(480, 640, CV_8UC1);
    
    // make binary image
    cv::threshold(m_outputImage, m_outputImage_thresh, 2, 255, cv::THRESH_BINARY);
    
    cv::Mat m_contour = cv::Mat(480, 640, CV_8UC1);
    m_outputImage_thresh.copyTo(m_contour);
    
    // find contures
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(m_contour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    
    hasLastTouch = false;
    // draw ellipses
    for (unsigned long i = 0; i < contours.size(); i++) {
        // less than 5 points throw an assertion error
        if (contours[i].size() >= 5) {
            cv::RotatedRect box = cv::fitEllipse(contours[i]);
            if (isFoot(contours[i])) {
                drawEllipse(box);
                lastTouch = box.center;
                hasLastTouch = true;
            }
        }
    }
    
    m_touchOutput = m_outputImage_thresh;
}

void Application::processFrame()
{	
	///////////////////////////////////////////////////////////////////////////
	//
	// To do:
	//
	// This method will be called every frame of the camera. Insert code here in
	// order to recognize touch and select the according game units.
	// These images will help you doing so:
	//
	// * m_bgrImage: The image of the Kinect's color camera
	// * m_depthImage: The image of the Kinects's depth sensor
	// * m_gameImage: The undistorted image in which the UIST game is rendered.
	// * m_outputImage: The final image distorted in a way that it appears
	//                  undistorted on the floor.
	//
	///////////////////////////////////////////////////////////////////////////

	// Sample code brightening up the depth image to make the values visible
    
    if (!hasLastTouch) {
        return;
    }
    
    cout << lastTouch << endl;
    
//    vector<Point3_<int>> homoVec;
//    Point3_<int> homoTouch = Point3_<int>(lastTouch.x, lastTouch.y, 1);
//    homoVec.push_back(homoTouch);
    
    Mat homoMat = Mat(1,3,CV_64F);
    homoMat.at<double>(0, 0) = (double)lastTouch.x;
    homoMat.at<double>(0, 1) = (double)lastTouch.y;
    homoMat.at<double>(0, 2) = (double)1;
    cout << homoMat << endl;
    
    
    Mat homography = m_calibration->cameraToPhysical() * m_calibration->physicalToProjector();
    cout << homography << endl;

    Mat homoTouchInUist = (homoMat * m_calibration->cameraToPhysical()) * m_calibration->physicalToProjector();
    
    Point2f touchInUist = Point(homoTouchInUist.at<double>(0), homoTouchInUist.at<double>(1));
    
    Point final = Point((int)touchInUist.x, (int)touchInUist.y);

    circle(m_outputImage, final, 2, Scalar(200,0,0));
    cout << final << endl;
}

void Application::processSkeleton(XnUserID userId)
{
	///////////////////////////////////////////////////////////////////////////
	//
	// To do:
	//
	// This method will be called every frame of the camera for each tracked user.
	// Insert code here in order to fulfill the assignment.
	//
	// Below is an example how to retrieve a skeleton joint position and
	// (if needed) how to convert its position into image space.
	//
	///////////////////////////////////////////////////////////////////////////

	m_skeletonTracker->drawSkeleton(m_bgrImage, userId);

	xn::SkeletonCapability skeletonCap = m_skeletonTracker->getSkeletonCapability();

	XnSkeletonJointPosition leftHand;
	skeletonCap.GetSkeletonJointPosition(userId, XN_SKEL_LEFT_HAND, leftHand);
	cv::Point2f imagePosition = m_skeletonTracker->getProjectedPosition(leftHand);
}

void Application::loop()
{
	int key = cv::waitKey(20);

	// If projector and camera aren't calibrated, do this and nothing else
	if (!m_calibration->hasTerminated())
	{
		if (key == 'q')
			m_isFinished = true;

		if(m_depthCamera)
		{
			m_depthCamera->getFrame(m_bgrImage, m_depthImage);
		}
		m_calibration->loop(m_bgrImage, m_depthImage);

		return;
	}

	switch (key)
	{
	case 'q': // quit
		m_isFinished = true;
		break;
	case 'p': // screenshot
		makeScreenshots();
		break;
	// run the loaded level
	case 'r':
		if(m_gameServer)
			m_gameServer->startGame();
		break;
	// Move fist (0) game unit with wasd
	case 'w': // north
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->moveUnit(0, (float)M_PI_2, 1.f);
		break;
	case 'a': // west
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->moveUnit(0, (float)M_PI, 1.f);
		break;
	case 's': // south
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->moveUnit(0, 3 * (float)M_PI_2, 1.f);
		break;
	case 'd': // east
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->moveUnit(0, 0.f, 1.f);
		break;
	// stop the first (0) game unit
	case ' ':
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->moveUnit(0, 0.f, 0.f);
		break;
	// highlight the first (0) game unit
	case 'h':
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->highlightUnit(0, true);
		break;
	case 'u':
		if(m_gameClient && m_gameClient->game())
			m_gameClient->game()->highlightUnit(0, false);
		break;
	}
	
	if(m_isFinished) return;

	if(m_gameClient && m_gameClient->game())
		m_gameClient->game()->render(m_gameImage);

	if(m_depthCamera)
	{
		m_depthCamera->getFrame(m_bgrImage, m_depthImage);
        
//        cv::flip(m_bgrImage, m_bgrImage, -1);
//        cv::flip(m_depthImage, m_depthImage, -1);
        
        if (!captured_reference) {
            m_depthImage.copyTo(m_reference);
            m_reference *= brighten_factor;
            captured_reference = true;

//            Mat homography = m_calibration->cameraToPhysical(); //* m_calibration->physicalToProjector();
//            warpPerspective(m_reference, m_reference, homography, Size(640, 480));
        }
        
        m_depthImageUntransformed = Mat(480, 640, CV_16UC1);
        
//        warpCameraToUntransformed();
        processTouch();
		processFrame();
//        cv::flip(m_touchOutput, m_touchOutput, 0);
//        warpUntransformedToTransformed();
        
//        cv::flip(m_bgrImage, m_bgrImage, 0);
//        cv::flip(m_depthImage, m_depthImage, 0);

	}

	if(m_skeletonTracker)
	{
		std::set<XnUserID>& users = m_skeletonTracker->getTrackedUsers();
		for (std::set<XnUserID>::iterator i = users.begin(); i != users.end(); ++i)
			processSkeleton(*i);
	}

	cv::imshow("bgr", m_bgrImage);
	cv::imshow("depth", m_depthImage);
	cv::imshow("output", m_outputImage);
    cv::imshow("touch", m_touchOutput);
	cv::imshow("UIST game", m_gameImage);
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
	, m_skeletonTracker(nullptr)
	, m_gameClient(nullptr)
	, m_gameServer(nullptr)
	, m_calibration(nullptr)
{
	// If you want to control the motor / LED
	// m_kinectMotor = new KinectMotor;

	// Not used for UIST game demo, uncomment for skeleton assignment
	m_depthCamera = new DepthCamera;
	// m_skeletonTracker = new SkeletonTracker(m_depthCamera);

	// open windows
	cv::namedWindow("output", CV_WINDOW_NORMAL);
	cv::namedWindow("depth", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("bgr", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("UIST game", CV_WINDOW_AUTOSIZE);

	// create work buffer
	m_bgrImage = cv::Mat(480, 640, CV_8UC3);
	m_depthImage = cv::Mat(480, 640, CV_16UC1);
	m_outputImage = cv::Mat(640, 480, CV_8UC1);
	m_gameImage = cv::Mat(480, 480, CV_8UC3);
    m_reference = cv::Mat(480, 640, CV_16UC1);
    m_depthImageUntransformed = cv::Mat(480, 640, CV_16UC1);
    m_touchOutput = cv::Mat(480, 640, CV_8UC1);

	if(uist_server == "127.0.0.1") {
		m_gameServer = new GameServer;
		m_gameClient = new GameClient;
		m_gameServer->run();
		m_gameServer->loadGame(uist_level);
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}
	m_gameClient->run();
	m_gameClient->connectToServer(uist_server);
	std::cout << "[Info] Connected to " << uist_server << std::endl;

	m_calibration = new Calibration();
}

Application::~Application()
{
	// Fails due to race conditions in UIST game
	/*
	if (m_gameClient)
	{
		m_gameClient->stop();
		delete m_gameClient;
	}
	if (m_gameServer)
	{
		m_gameServer->stop();
		delete m_gameServer;
	}*/

	if (m_skeletonTracker) delete m_skeletonTracker;
	if (m_depthCamera) delete m_depthCamera;
	if (m_kinectMotor) delete m_kinectMotor;
	if (m_calibration) delete m_calibration;
}

bool Application::isFinished()
{
	return m_isFinished;
}
