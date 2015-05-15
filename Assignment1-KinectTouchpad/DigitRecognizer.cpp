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


DigitRecognizer::DigitRecognizer(): m_digitImage(nullptr)
{
    // load dataset
}

int DigitRecognizer::recognizeDigit(vector<Point> pointVector) {
    
    assert(pointVector.size() > 0);
    
    vector<Point> simplifiedPointVector = vector<Point>();
    
    approxPolyDP(pointVector, simplifiedPointVector, 3, false);
    
    // downsampling
    vector<Point> samplePoints = vector<Point>();
    sample(simplifiedPointVector, samplePoints, 8);
    
    // debug output
    (*m_digitImage) = cv::Mat(480, 640, CV_8UC3);
    drawLine(samplePoints);
    
    return 1;
}

float getLengthOfLine(Point a, Point b) {
    return norm(a-b);
}

float getLengthOfLines(vector<Point> line) {
    float length = 0;
    
    for (int i = 0; i < line.size() - 1; i++) {
        length += getLengthOfLine(line[i], line[i+1]);
    }
    
    return length;
}

Point getPointInDirection(Point from, Point to, float distance) {
    return (distance / getLengthOfLine(from, to)) * (to - from) + from;
}

void DigitRecognizer::sample(vector<Point> input, vector<Point> output, int pointCount)
{
    // initialize first point
    output.push_back(input[0]);
    
    // get length
    float length = getLengthOfLines(input);
    
    // length chunk
    float lengthChunk = length / (float) (pointCount - 1);
    float lengthBuffer = lengthChunk;
    
    Point from;
    Point to;
    float restLength;
    
    for (int i = 0; i < input.size() - 1; i++) {
        from = input[i];
        to = input[i+1];
        restLength = getLengthOfLine(from, to);
        
        
        while (restLength >= lengthBuffer) {
            // get point on the line and put to output array
            restLength -= lengthBuffer;
            from = getPointInDirection(from, to, getLengthOfLine(from, to) - restLength);
            output.push_back(from);
            
            // line chunk was reached... reset buffer
            lengthBuffer = lengthChunk;
        }
        
        lengthBuffer -= restLength;
    }
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
            line(*m_digitImage, from, to, line_color, 4);
        }
    }
}
