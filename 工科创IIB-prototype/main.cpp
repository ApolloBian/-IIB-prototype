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

extern int fd;
extern char dev1[];
extern char dev2[];


int main() {
    
// connecting car
    
    
//    int ret;
//    fd = UART_Open(fd,dev2);
//    ret = UART_Init(fd,115200,0,8,1,'N');
//
//    while (fd == -1) {
//        fd = UART_Open(fd,dev2);
//        ret = UART_Init(fd,115200,0,8,1,'N');
//    }
//    stop(fd);
    
    
    
// capture camera
    extern IplImage * windowImage;
    extern IplImage * processImage;
    CvCapture * cam;
    cam = cvCreateCameraCapture(1);
    cvNamedWindow("monitor");
    if (!cam) {
        return -1;
    }
    while (1) {
        windowImage = cvQueryFrame(cam);
        if (cvWaitKey(50) == '\r') {
//            windowImage = cvLoadImage("/Users/apollo/Desktop/屏幕快照 2015-12-09 下午1.40.09.png");
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
    controlling(fd);
    

    cvReleaseCapture(&cam);
    return 0;
    
}