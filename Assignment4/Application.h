#pragma once

#include <opencv2/core/core.hpp>
#include <boost/tokenizer.hpp>
#include <XnTypes.h>

class GameClient;
class GameServer;

class DepthCamera;
class KinectMotor;
class SkeletonTracker;

class Calibration;

class Application
{
public:
	Application();
	virtual ~Application();

	void loop();

	void warpCameraToUntransformed();
    void warpUntransformedToTransformed();
	void processFrame();
	void processSkeleton(XnUserID userId);
    void processTouch();
    bool isFoot(std::vector<cv::Point> contour);
    void drawEllipse(cv::RotatedRect box);

	void makeScreenshots();
	void clearOutputImage();

	bool isFinished();

protected:
	GameClient *m_gameClient;
	GameServer *m_gameServer;

	DepthCamera *m_depthCamera;
	KinectMotor *m_kinectMotor;
	SkeletonTracker *m_skeletonTracker;

	Calibration *m_calibration;

	cv::Mat m_bgrImage;
	cv::Mat m_depthImage;
    cv::Mat m_depthImageUntransformed;
	cv::Mat m_outputImage;
	cv::Mat m_gameImage;
    cv::Mat m_reference;
    cv::Mat m_touchOutput;

	bool m_isFinished;

	static const int uist_level;
	static const char *uist_server;
};