//
//  perspectiveTransform.cpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/3/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#include "perspectiveTransform.h"

CvPoint2D32f originalPoints[4];
CvPoint2D32f sequentialPoints[4];
CvPoint2D32f transPoints[4];
CvMat * transmat = cvCreateMat(3, 3, CV_32FC1 );
int perspectiveX;
int perspectiveY;
int perspectivecount;

void setVariables() {
    transPoints[0] = cvPoint2D32f(10, 10);
    transPoints[1] = cvPoint2D32f(390, 10);
    transPoints[2] = cvPoint2D32f(10, 390);
    transPoints[3] = cvPoint2D32f(390, 390);
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
    cvSetMouseCallback("control", perspective_mouse);
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

