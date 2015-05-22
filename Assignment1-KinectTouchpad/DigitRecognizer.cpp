//
//  DigitRecognizer.cpp
//  HCI2Assignments
//
//  Created by Sven Mischkewitz on 15/05/15.
//  Copyright (c) 2015 Sven Mischkewitz. All rights reserved.
//

#include "DigitRecognizer.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include "DataSet.h"

using namespace cv;
using namespace std;

static cv::Scalar line_color = cv::Scalar(0,0,255);


DigitRecognizer::DigitRecognizer(): m_digitImage(nullptr)
{
    // adapt row count!
    readDataSet("/Users/nico/Studium/HCI2/Coding/HCI2-Assignments/Assignment1-KinectTouchpad/pendigits.tra", rowCount, data, labels);
//    cout << data << endl;
}

int sortDistances(Point2f a, Point2f b) {
    return a.x < b.x;
}

pair<int, float> DigitRecognizer::recognizeDigit(vector<Point> pointVector) {
    
    assert(pointVector.size() > 0);
    
    vector<Point> simplifiedPointVector = vector<Point>();
    
    approxPolyDP(pointVector, simplifiedPointVector, 3, false);
    
    // downsampling
    vector<Point> samplePoints = vector<Point>();
    sample(simplifiedPointVector, &samplePoints, pointCount);
    
    // debug output
//    (*m_digitImage) = cv::Mat(480, 640, CV_8UC3);
    drawLine(samplePoints);
    
    vector<Point> bbox = getBBox(samplePoints);
    vector<Point2f> normalizedPoints;
    normalizePoints(&samplePoints, &normalizedPoints, bbox);
//    cout << normalizedPoints << endl;
    
    vector<float> flat = vector<float>();
    for (Point2f p : normalizedPoints) {
        flat.push_back(p.x);
        flat.push_back(p.y);
    }
            
    vector<Point2f> distances = vector<Point2f>();
    int k = kCount;
    
    // gather all distances
    for (int row = 0; row < rowCount; row++) {
        distances.push_back(Point2f(norm(data.row(row), flat, NORM_L2), labels.at<float>(row)));
    }
    
    // sort
    sort(distances.begin(), distances.end(), sortDistances);
    
    // get k nearest
    vector<Point2f> kNearest = vector<Point2f>(distances.begin(), distances.begin()+k);
    
    // voting
    int votes[10] = { 0 };
    for (Point2f p : kNearest) {
        votes[(int)p.y]++;
    }
    
    int max = -1;
    int score = -1;
    for (int i = 0; i < 10; i++) {
        cout << votes[i] << endl;
        if (votes[i] > score) {
            score = votes[i];
            max = i;
        }
    }
    float precision = score / (float)k;
    cout << "max: " << max << endl;
    
    
//    PointsToMat(normalizedPoints, dataMat);
//    cout << dataMat << endl;
    
    return pair<int, float>(max, precision);
}

vector<Point> DigitRecognizer::getBBox(vector<Point> points)
{
    Point bottomLeft = points[0];
    Point topRight = points[0];
    Point current;
    
    for (int i = 1; i < points.size(); i++) {
        current = points[i];
        
        if (current.x < bottomLeft.x ) {
            bottomLeft.x = current.x;
        } else if (current.x > topRight.x) {
            topRight.x = current.x;
        }
        
        if (current.y < bottomLeft.y) {
            bottomLeft.y = current.y;
        } else if (current.y > topRight.y) {
            topRight.y = current.y;
        }
    }
    
    return {bottomLeft, topRight};
}

void DigitRecognizer::normalizePoints(vector<Point> *points, vector<Point2f> *normalizedPoints, vector<Point> bbox)
{
    Point shiftToZero = bbox[0];
    Point scaleToOne = bbox[1] - shiftToZero;
    
    Point2f tempPoint;
    for (int i = 0; i < points->size(); i++) {
        tempPoint = (points->at(i) - shiftToZero);
        tempPoint.x /= (float)scaleToOne.x;
        tempPoint.y /= (float)scaleToOne.y;
        normalizedPoints->push_back(tempPoint);
    }
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

void DigitRecognizer::PointsToMat(vector<Point2f> points, Mat &mat) {
    mat = cv::Mat(1, 16, CV_32FC1);
    cout << "mat before" << mat << endl;
    for (int i = 0, j = 0; i < points.size(); i++, j+=2) {
        cout << i << " " << j << endl;
        mat.at<float>(1, j) = points[i].x;
        mat.at<float>(1, j+1) = points[i].y;
    }
}

int findDaKaNearestN() {
    return 1;
}

void DigitRecognizer::sample(vector<Point> input, vector<Point> *output, int pointCount)
{
    // initialize first point
    output->push_back(input[0]);
    
    // get length
    float length = getLengthOfLines(input);
    
    // length chunk
    float lengthChunk = length / (float) (pointCount - 1);
    float lengthBuffer = lengthChunk;
//    cout << "\nlengthChunk: " << lengthChunk << "\n" << endl;
    Point from;
    Point to;
    float restLength = 0;
    long loopCount = 0;
    
    for (int i = 0; i < input.size() - 1; i++) {
        from = input[i];
        to = input[i+1];
        restLength = getLengthOfLine(from, to);
        loopCount = 0;
        
        while (restLength > lengthBuffer) {
//            loopCount++;
//            cout << "loopCount: " << loopCount << " restLength: " << restLength << " lengthBuffer: " << lengthBuffer << endl;
            // get point on the line and put to output array
            restLength -= lengthBuffer;
            from = getPointInDirection(from, to, getLengthOfLine(from, to) - restLength);
            output->push_back(from);
            
            // line chunk was reached... reset buffer
            lengthBuffer = lengthChunk;
            
        }
        
        lengthBuffer -= restLength;
    }
    
    if (output->size() < pointCount) {
        from = getPointInDirection(from, to, getLengthOfLine(from, to) - restLength);
        output->push_back(from);
    }
    
    assert(output->size() == pointCount);
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
