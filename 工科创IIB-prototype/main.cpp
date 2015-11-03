//
//  perspectiveTransform.hpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/3/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

// main control function for PCCC
#include <iostream>
#include "image.h"

using namespace std;





int main() {
// capture camera
    CvCapture * cam = cvCreateCameraCapture(-1);
    cvNamedWindow("video");
    if (!cam) {
        return -1;
    }
    IplImage * windowImage;
    IplImage * processImage;
    while (1) {
        windowImage = cvQueryFrame(cam);
        cvShowImage("video", windowImage);
        if (cvWaitKey(1) == '\r') {
            break;
        }
    }
// perspective transform
    
    
    
// binarization
    
// thin
    
// hough transformation
// see pdf page 27
    
// tracking
    
// control car
    
    
    
// quit program, release memory & other stuff
    cvReleaseCapture(&cam);
    return 0;
    
}