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
using namespace std;

//注:窗口是全局变量，无论在哪个文件中调用窗口的名字都是对同一个窗口进行操作




int main() {
    IplImage * windowImage;
    IplImage * processImage;
// capture camera
    CvCapture * cam = cvCreateCameraCapture(-1);
    cvNamedWindow("control");
    if (!cam) {
        return -1;
    }
    
    while (1) {
        windowImage = cvQueryFrame(cam);
        cvShowImage("control", windowImage);
        if (cvWaitKey(1) == '\r') {
            break;
        }
    }
    
    
    
// perspective transform, 变换一帧图像大约需要不到5ms(对于1020 × 790的图像需要3.577ms)
    
    extern CvPoint2D32f originalPoints[4];
    extern CvPoint2D32f sequentialPoints[4];
    extern CvPoint2D32f transPoints[4];
    extern CvMat * transmat;                                                                  //这个变量在后面还会用到
    processImage = windowImage;
    
    setVariables();                                                                           //设置四个预设的点
    setPoints();                                                                              //选取图片上的点
    windowImage = cvCreateImage(cvSize(400,400), IPL_DEPTH_8U, 3);
    cvGetPerspectiveTransform(originalPoints, transPoints, transmat);                         //计算transmat的值
    cvWarpPerspective(processImage , windowImage , transmat);                                 //这一句利用transmat进行变换
    cvShowImage("control", windowImage);
    cvWaitKey();
    
    // binarization
    //尝试cvInRangeS(destimage1, CV_RGB(100, 100, 100), CV_RGB(255, 255, 255), finalimage);双阈值
    processImage = windowImage;
    windowImage = cvCreateImage(cvGetSize(processImage), IPL_DEPTH_8U, 1);
    cvInRangeS(processImage, CV_RGB(100, 100, 100), CV_RGB(255, 255, 255), windowImage);
    cvShowImage("control", windowImage);

    
    
    cvWaitKey();
    
// thin
    
// hough transformation
// see pdf page 27
    
// tracking
    
// control car
    
    
    
// quit program, release memory & other stuff
    cvReleaseCapture(&cam);
    return 0;
    
}