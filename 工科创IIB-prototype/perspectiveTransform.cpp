//
//  perspectiveTransform.cpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/3/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#include "perspectiveTransform.h"

IplImage * processImage;
IplImage * windowImage;

CvPoint2D32f originalPoints[4];
CvPoint2D32f sequentialPoints[4];
CvPoint2D32f transPoints[4];
CvMat * transmat = cvCreateMat(3, 3, CV_32FC1 );
int perspectiveX;
int perspectiveY;
int perspectivecount;




void perspectiveTransform() {

    processImage = windowImage;
    
    setVariables();                                                                           //设置四个预设的点
    setPoints();                                                                              //选取图片上的点
    cvDestroyWindow("monitor");
    windowImage = cvCreateImage(cvSize(600,600), IPL_DEPTH_8U, 3);
    cvGetPerspectiveTransform(originalPoints, transPoints, transmat);                         //计算transmat的值
    cvWarpPerspective(processImage , windowImage , transmat);                                 //这一句利用transmat进行变换
    cvNamedWindow("control");
    cvMoveWindow("control", leftwindowX, leftwindowY);
    cvShowImage("control", windowImage);
    cvWaitKey();
    cvDestroyWindow("control");
}
void setVariables() {
    transPoints[0] = cvPoint2D32f(0, 0);
    transPoints[1] = cvPoint2D32f(squareWindowSize ,0);
    transPoints[2] = cvPoint2D32f(0, squareWindowSize);
    transPoints[3] = cvPoint2D32f(squareWindowSize,squareWindowSize);
}


void perspective_mouse(int mouseAction , int x , int y , int flags , void * param) {
    
    if (mouseAction == CV_EVENT_LBUTTONUP){
        perspectiveX = x;
        perspectiveY = y;
        ++perspectivecount;
    }
}

void setPoints() {
    perspectivecount = 0;
    cvSetMouseCallback("monitor", perspective_mouse);
    while (perspectivecount <= 3) {
        int temp = perspectivecount;
        cvWaitKey(30);
        if (temp < perspectivecount) {
            originalPoints[perspectivecount-1] = cvPoint2D32f(perspectiveX, perspectiveY);
        }
    }
    CvPoint2D32f temp;
    for (int i = 0 ; i < 3 ; ++i) {
        for (int j = 0 ; j < 3-i; ++j) {
            if (originalPoints[j].y > originalPoints[j+1].y) {
                temp = originalPoints[j];
                originalPoints[j]=originalPoints[j+1];
                originalPoints[j+1]=temp;
            }
        }
    }
    if (originalPoints[0].x > originalPoints[1].x) {
        temp = originalPoints[0];
        originalPoints[0]=originalPoints[1];
        originalPoints[1]=temp;
    }
    if (originalPoints[2].x > originalPoints[3].x) {
        temp = originalPoints[2];
        originalPoints[2]=originalPoints[3];
        originalPoints[3]=temp;
    }
}

