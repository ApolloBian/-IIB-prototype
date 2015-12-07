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
#include "controlling.h"
using namespace std;

//注:窗口是全局变量，无论在哪个文件中调用窗口的名字都是对同一个窗口进行操作




int main() {
    
// capture camera
    extern IplImage * windowImage;
    extern IplImage * processImage;
    CvCapture * cam;
    cam = cvCreateCameraCapture(-
                                1);
    cvNamedWindow("monitor");
    if (!cam) {
        return -1;
    }
    while (1) {
        windowImage = cvQueryFrame(cam);
        if (cvWaitKey(50) == '\r') {

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
    
// tracking
    controlling();
    

    cvReleaseCapture(&cam);
    return 0;
    
}