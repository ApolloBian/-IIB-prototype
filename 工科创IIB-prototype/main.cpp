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
#include "perspectiveTransform.h"
#include "binarization.h"
#include "hough.h"
using namespace std;





//注:窗口是全局变量，无论在哪个文件中调用窗口的名字都是对同一个窗口进行操作




int main() {
    
// capture camera
    extern IplImage * windowImage;
    extern IplImage * processImage;
    CvCapture * cam = cvCreateCameraCapture(-1);
    cvNamedWindow("monitor");
    if (!cam) {
        return -1;
    }
    while (1) {
        windowImage = cvQueryFrame(cam);
        if (cvWaitKey(1) == '\r') {
            windowImage = cvLoadImage("/Users/apollo/Desktop/Screen Shot 2015-10-21 at 6.00.27 PM.png");
            cvShowImage("monitor", windowImage);
            break;
        }
        cvShowImage("monitor", windowImage);
    }
    
    
    
// perspective transform, 变换一帧图像大约需要不到5ms(对于1020 × 790的图像需要3.577ms)
    extern CvMat * transmat;                                                                  //这个变量在后面还会用到
    perspectiveTransform();
    bin();
    thin();
    hough();

// see pdf page 27
    
// tracking
    
// control car
    
    
    
// quit program, release memory & other stuff
    cvReleaseCapture(&cam);
    return 0;
    
}