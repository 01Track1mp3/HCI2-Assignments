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

using namespace cv;
using namespace std;

int DigitRecognizer::recognizeDigit(vector<Point> pointVector) {
    
    assert(pointVector.size() > 0);
    
    vector<Point> simplifiedPointVector = vector<Point>();
    
    approxPolyDP(pointVector, simplifiedPointVector, 3, false);
    cout << pointVector.size() << " " << simplifiedPointVector.size() << endl;
    
    return 1;
}

void drawLastLine(vector<Point> subVector)
{
    cv::Point from;
    cv::Point to;
    
    if (subVector.size() > 2) {
        for (int j = 0; j < subVector.size() - 1; j++) {
            from = subVector[j];
            to = subVector[j + 1];
            cv::line(m_bgrImage, from, to, line_color, 4);
        }
    }
}
