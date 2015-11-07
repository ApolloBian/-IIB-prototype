//
//  hough.cpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/7/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#include "hough.h"
#include <iostream>


extern IplImage * binGrayImage;

int threshold = 0;
int minLength = 0;
int maxDistance = 0;
CvSeq* lines;
IplImage * srcImage;
CvMemStorage * storage;
IplImage * tarImage;

void checkParameter() {
    if (threshold <= 0) {
        threshold = 1;
    }
    if (minLength <= 0 ) {
        minLength = 1;
    }
    if (maxDistance <= 0) {
        maxDistance = 1;
    }
    
}

void performHough() {
    checkParameter();
//    threshold = 62
//    minLength = 11
//    maxDistance = 53
    
//    threshold = 22
//    minLength = 25
//    maxDistance = 17
    
//    threshold = 44
//    minLength = 71
//    maxDistance = 36
    
//    threshold = 44
//    minLength = 80
//    maxDistance = 37
    
    lines = cvHoughLines2(srcImage, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, threshold, minLength, maxDistance);
    cvSetZero(tarImage);
    for (int i = 0 ; i < lines->total; ++i) {
        CvPoint * line = (CvPoint * )cvGetSeqElem(lines , i );
        cvLine(tarImage , line[0], line[1], cvScalar(255));
    }
}

void showParameters() {
    std::cerr<<"threshold = "<<threshold<<std::endl;
    std::cerr<<"minLength = "<<minLength<<std::endl;
    std::cerr<<"maxDistance = "<<maxDistance<<std::endl;
}



void houghChangeThreshold(int pos) {
    threshold = pos;
    showParameters();
    performHough();
    cvShowImage("hough", tarImage);
}


void houghChangeMinLength(int pos) {
    minLength = pos;
    showParameters();
    performHough();
    cvShowImage("hough", tarImage);
}

void houghChangeMaxDistance(int pos) {
    maxDistance = pos;
    showParameters();
    performHough();
    cvShowImage("hough", tarImage);
}

void hough() {
    cvNamedWindow("hough");
    cvMoveWindow("hough", middlewindowX, middlewindowY);
    srcImage = binGrayImage;
    storage = cvCreateMemStorage();
    tarImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 1);
    cvShowImage("hough", srcImage);
    cvCreateTrackbar("threshold", "hough", &threshold, 100,houghChangeThreshold);
    cvCreateTrackbar("minLength", "hough", &minLength, 100,houghChangeMinLength);
    cvCreateTrackbar("maxDistance", "hough", &maxDistance, 100,houghChangeMaxDistance);
    cvWaitKey(0);
    
}

