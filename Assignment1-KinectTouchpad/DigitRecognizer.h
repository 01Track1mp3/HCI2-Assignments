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
        static int recognizeDigit(std::vector<cv::Point> pointVector);
    

    
};


#endif /* defined(__HCI2Assignments__DigitRecognizer__) */
