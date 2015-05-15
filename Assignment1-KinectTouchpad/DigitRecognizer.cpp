//
//  DigitRecognizer.cpp
//  HCI2Assignments
//
//  Created by Sven Mischkewitz on 15/05/15.
//  Copyright (c) 2015 Sven Mischkewitz. All rights reserved.
//

#include "DigitRecognizer.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "DataSet.h"

using namespace cv;
using namespace std;

static cv::Scalar line_color = cv::Scalar(0,0,255);

int DigitRecognizer::recognizeDigit(vector<Point> pointVector) {
    
    assert(pointVector.size() > 0);
    
    vector<Point> simplifiedPointVector = vector<Point>();
    
    approxPolyDP(pointVector, simplifiedPointVector, 3, false);
    cout << pointVector.size() << " " << simplifiedPointVector.size() << endl;
    
    drawLine(simplifiedPointVector);
}

void DigitRecognizer::drawLine(vector<Point> lineVector)
{
    Point from;
    Point to;
    
    if (lineVector.size() > 2) {
        for (int j = 0; j < lineVector.size() - 1; j++) {
            from = lineVector[j];
            to = lineVector[j + 1];
            
            // draw the line
            line(m_digitImage, from, to, line_color, 4);
        }
    }
}
