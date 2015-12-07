#include "controlling.h"
#include <iostream>
extern CvMat * transmat;

CvHistogram* carFrontHistogram;
CvHistogram* carBackHistogram;


//int vmin = 50;
//int vmax = 256;
//int smin = 80;

int vmin = 10;
int vmax = 256;
int smin = 30;

void controlling() {
    
    
    
    
    
    
    CvCapture * cam = cvCreateCameraCapture(-1);
    IplImage * cameraImage;
    IplImage * monitorImage = cvCreateImage(cvSize(squareWindowSize, squareWindowSize), IPL_DEPTH_8U, 3);
    IplImage * hsv , * hue , * mask , * backprojectF, *backprojectB;
    hsv = cvCreateImage(cvGetSize(monitorImage), IPL_DEPTH_8U, 3);
    hue = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    mask = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectF = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectB = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);

    CvRect track_windowF = {0,0,squareWindowSize,squareWindowSize};
    CvRect track_windowB = {0,0,squareWindowSize,squareWindowSize};
    CvBox2D track_boxF, track_boxB;
    CvConnectedComp track_compF,track_compB;
    
    

    calcHistogram(carfront,carFrontHistogram);
    calcHistogram(carback, carBackHistogram);
    
    //不进行任何操作，放小车
    cvNamedWindow(monitor);
    cvMoveWindow(monitor, leftwindowX, leftwindowY);
    
    
    
    //放完小车之后分别进行前后端追踪的参数调整
//    cvNamedWindow("front");
//    cvMoveWindow("front", rightwindowX, rightwindowY);
//    cvCreateTrackbar( "Vmin", "front", &vmin, 256, 0 );
//    cvCreateTrackbar( "Vmax", "front", &vmax, 256, 0 );
//    cvCreateTrackbar( "Smin", "front", &smin, 256, 0 );
    int _vmin = vmin, _vmax = vmax,_smin = smin;

    while (true) {
        cameraImage = cvQueryFrame(cam);
        cvWarpPerspective(cameraImage, monitorImage, transmat);
        cvCvtColor(monitorImage, hsv, CV_RGB2HSV);
        cvInRangeS( hsv, cvScalar(0,_smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0), mask );                                                               //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
        cvSplit( hsv, hue, 0, 0, 0 );//分离H分量
        
        
        
        
        cvCalcBackProject(&hue, backprojectF , carFrontHistogram);
        cvAnd(backprojectF, mask, backprojectF,0);
        
        cvCamShift( backprojectF, track_windowF,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_compF, &track_boxF );
        track_windowF = track_compF.rect;
        cvRectangleR(monitorImage,track_windowF, CV_RGB(255,0,0),3,CV_AA,0);

        cvCalcBackProject(&hue, backprojectB, carBackHistogram);
        cvAnd(backprojectB, mask, backprojectB,0);
        cvCamShift( backprojectB, track_windowB,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_compB, &track_boxB );
        track_windowB = track_compB.rect;
        cvRectangleR(monitorImage,track_windowB, CV_RGB(255,0,0),3,CV_AA,0);
        
        cvShowImage(monitor, monitorImage);
        cvShowImage(trackwindowName, backprojectF);
        //        cvShowImage(trackwindowName, mask);
        if (cvWaitKey(33) == '\r') {
            break;
        }
    }
    
    
}