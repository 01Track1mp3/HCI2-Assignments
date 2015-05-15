//
//  DigitRecognizer.h
//  HCI2Assignments
//
//  Created by Sven Mischkewitz on 15/05/15.
//  Copyright (c) 2015 Sven Mischkewitz. All rights reserved.
//

#ifndef __HCI2Assignments__DigitRecognizer__
#define __HCI2Assignments__DigitRecognizer__

#include <stdio.h>
#include <opencv2/core/core.hpp>

class DigitRecognizer
{

public:
    DigitRecognizer();
    int recognizeDigit(std::vector<cv::Point> pointVector);
    void drawLine(std::vector<cv::Point> lineVector);
    void sample(std::vector<cv::Point> input, std::vector<cv::Point> *output, int pointCount);
    cv::Mat *m_digitImage;
    
protected:
    std::vector<cv::Point> getBBox(std::vector<cv::Point> points);
    void normalizePoints(std::vector<cv::Point> *points, std::vector<cv::Point2f> *normalizedPoints, std::vector<cv::Point> bbox);

};


#endif /* defined(__HCI2Assignments__DigitRecognizer__) */
