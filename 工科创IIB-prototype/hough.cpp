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
extern IplImage * windowImage;


int threshold = 22;
int minLength = 45;
int maxDistance = 70;
CvSeq* lines;
carSeq path;
int numberOfPathPoints = 0;


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

    
    
    
    
    
    lines = cvHoughLines2(srcImage, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, threshold, minLength, maxDistance);
    tarImage = cvCloneImage(windowImage);
    CvRect rect;
    rect.width = 10;
    rect.height = 10;
    path.build(lines);
    print(path, tarImage);
    
    for(int i = 0; i < lines->total; i++) {
        CvPoint* line = (CvPoint*) cvGetSeqElem(lines, i);
        cvLine( tarImage, line[0], line[1], cvScalar(255));
    }
 
    
//    for (int i = 0 ; i < lines->total; ++i) {
//        CvPoint * line = path.line[i];
//        rect.x = line[0].x-5;
//        rect.y = line[0].y-5;
//        cvRectangleR(tarImage,rect, CV_RGB(255,0,0),3,CV_AA,0);
//        rect.x = line[1].x-5;
//        rect.y = line[1].y-5;
//        cvRectangleR(tarImage,rect, CV_RGB(255,0,0),3,CV_AA,0);
//    }
//    CvPoint * line = path.line[path.entranceIndex];
//    rect.x = line[path.entranceEnd].x-5;
//    rect.y = line[path.entranceEnd].y-5;
//    cvRectangleR(tarImage,rect, CV_RGB(0,255,0),3,CV_AA,0);

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
//    tarImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 3);
    tarImage = cvCloneImage(windowImage);
    cvShowImage("hough", tarImage);
    cvCreateTrackbar("threshold", "hough", &threshold, 100,houghChangeThreshold);
    cvCreateTrackbar("minLength", "hough", &minLength, 100,houghChangeMinLength);
    cvCreateTrackbar("maxDistance", "hough", &maxDistance, 100,houghChangeMaxDistance);
    
    cvWaitKey(0);
//    while (true) {
//        cvWaitKey(0);
//        for (int i = 0 ; i < lines->total; ++i) {
//            CvPoint * line = lineArray[i] = (CvPoint * )cvGetSeqElem(lines , i );
//            cvLine(tarImage , line[0], line[1], cvScalar(255));
//        }
//        
//        // 检查相同的线，此处用到了阈值，在image那个文件里改
///*
//        for (int i = 0 ; i < numbers-1; ++i) {
//            for (int j = 0 ; j < numbers ; ++j) {
//                if (i == j) {
//                    continue;
//                }
//                if (sameLine(lineArray[i], lineArray[j])) {
//                    for (int m = j+1 ; m < numbers; ++m) {
//                        lineArray[m-1] = lineArray[m];
//                    }
//                    --j;
//                    --numbers;
//                    if (j < i) {
//                        --i;
//                    }
//                }
//                
//            }
//        }
//        std::cout<<"numbers = "<<numbers;
//        numberOfPathPoints = 2*numbers;
//        
//        
//        
//        int minDis = 100000;
//        int maxD = 0;
//        int startLineIndex = -1;
//        int startPointIndex = -1;
//        for (int i = 0 ; i < numbers; ++i) {
//            for (int j = 0 ; j < 2 ; ++j) {
//                minDis = 100000;
//                for (int k = 0 ; k < numbers ; ++k) {
//                    if (k == i) {
//                        continue;
//                    }
//                    int dis1 = length(lineArray[i][j],lineArray[k][0]);
//                    int dis2 = length(lineArray[i][j],lineArray[k][1]);
//                    if (minDis < MIN(dis1, dis2)) {
//                        minDis = MIN(dis1, dis2);
//                    }
//                }
//                if (minDis > maxD) {
//                    maxD = minDis;
//                    startLineIndex = i;
//                    startPointIndex = j;
//                }
//            }
//        }
//        
//        cvSetZero(tarImage);
//        for (int i = 0 ; i < numbers; ++i) {
//            cvLine(tarImage , lineArray[i][0], lineArray[i][1], cvScalar(255));
//        }
//        
//        cvShowImage("hough", tarImage);
//        std::cout<<numbers;
// */
//        if (cvWaitKey(0) == 13) {
//            cvDestroyAllWindows();
//            break;
//        }
//    }

}

