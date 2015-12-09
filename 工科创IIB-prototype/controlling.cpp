#include "controlling.h"
#include <iostream>
#include <cmath>
#include "timecalc.h"

using namespace cv;

double COS  = cos(M_PI/40);


extern CvMat * transmat;

CvHistogram* carFrontHistogram;
CvHistogram* carBackHistogram;

extern int fd;

//int vmin = 50;
//int vmax = 256;
//int smin = 80;

int vmin = 50;
int vmax = 256;
int smin = 80;

extern CvSeq* lines;

extern CvPoint * lineArray[20];

extern carSeq path;


//本地变量
CvCapture * cam = cvCreateCameraCapture(1);
IplImage * cameraImage;
IplImage * monitorImage = cvCreateImage(cvSize(squareWindowSize, squareWindowSize), IPL_DEPTH_8U, 3);
IplImage * hsv , * hue , * mask , * backprojectF, *backprojectB;


CvRect track_windowF = {0,0,squareWindowSize,squareWindowSize};
CvRect track_windowB = {0,0,squareWindowSize,squareWindowSize
};
CvBox2D track_boxF, track_boxB;
CvConnectedComp track_compF,track_compB;

int _vmin = vmin, _vmax = vmax,_smin = smin;
CvPoint pointB, pointF;
CvPoint midPoint;


void getNewFrame() {                                                                //
    cameraImage = cvQueryFrame(cam);
    cvWarpPerspective(cameraImage, monitorImage, transmat);
}

void locateCar() {                                                                  //追踪小车，更新图像,更新小车的位置变量
    cvCvtColor(monitorImage, hsv, CV_RGB2HSV);
    cvInRangeS(hsv,cvScalar(0,_smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0),mask);                                                               //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
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
    
    pointB.x = track_windowB.x+track_windowB.width/2;
    pointB.y = track_windowB.y+track_windowB.height/2;
    
    pointF.x = track_windowF.x+track_windowF.width/2;
    pointF.y = track_windowF.y+track_windowF.height/2;
    cvLine(monitorImage, pointB, pointF ,CV_RGB(255,0,0),3,CV_AA,0);
   
}

timeval a,b;

bool refreshPathStatus(){
    gettimeofday(&a, NULL);
    midPoint.x = pointB.x +pointF.x;
    midPoint.y = pointF.y + pointB.y;
    for (int i = 0 ; i < path.numberOfLines; ++i) {
        for (int j = 0 ; j < 2 ; ++j) {
            if (path.lineStatus[i][j]) {
                continue;
            }
            if (length(midPoint, path.line[i][j]) < 100) {
                path.lineStatus[i][j] = true;
            }
            
        }
    }
    if (path.lineStatus[path.currentIndex][path.currentEnd]) {
        double minTurn = -1;
        double curTurn = 0;
        double x1 = pointF.x-pointB.x;
        double y1 = pointF.y-pointB.y;
        double x2 = 1;
        double y2 = 1;
        bool flag = false;
        for (int i = 0 ; i < path.numberOfLines; ++i) {
            for (int j = 0 ; j < 2 ; ++j) {
                if (path.lineStatus[i][j]) {
                    continue;
                }
                x2 = path.line[i][j].x-midPoint.x;
                y2 = path.line[i][j].y-midPoint.y;
                curTurn = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
                if (curTurn > minTurn) {
                    flag = true;
                    minTurn = curTurn;
                    path.currentIndex = i;
                    path.currentEnd = j;
                }
            }
        }
        if (!flag) {
            path.reset();
        }
        gettimeofday(&b, NULL);
        myTime::timeCalc(a, b);
        return true;
    }
    gettimeofday(&b, NULL);
    myTime::timeCalc(a, b);
    return false;
    
    
}


void turnToNextPoint() {
    double x1 = pointF.x-pointB.x;
    double y1 = pointF.y-pointB.y;
    double x2 = 1;
    double y2 = 1;
    double theta = -1;
    x2 = path.line[path.currentIndex][path.currentEnd].x-midPoint.x;
    y2 = path.line[path.currentIndex][path.currentEnd].y-midPoint.y;
    theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
    
    if (y1*x2-y2*x1 > 0) {
        turnright(fd);
    } else {
        turnleft(fd);
    }
    
    while (theta <= COS) {
        getNewFrame();
        locateCar();
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);
        x2 = path.line[path.currentIndex][path.currentEnd].x-midPoint.x;
        y2 = path.line[path.currentIndex][path.currentEnd].y-midPoint.y;
        x1 = pointF.x-pointB.x;
        y1 = pointF.y-pointB.y;
        theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
    }
    stop(fd);
    getNewFrame();
    locateCar();
}



void controlling(int fd) {
    // 地图目标点
    // 图像追踪部分的变量
    hsv = cvCreateImage(cvGetSize(monitorImage), IPL_DEPTH_8U, 3);
    hue = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    mask = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectF = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectB = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    
    

    calcHistogram(carfront,carFrontHistogram);
    calcHistogram(carback, carBackHistogram);
    cvNamedWindow(monitor);
    cvMoveWindow(monitor, leftwindowX, leftwindowY);
    
    
//试验前调整，防止追踪不到
    while (true) {
        getNewFrame();
        locateCar();
        refreshPathStatus();
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);
        if (cvWaitKey(33) == '\r') {
            break; 
        }
    }
    
    cvWaitKey(1000);
    
    getNewFrame();
    locateCar();
    refreshPathStatus();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);
//    turnToNextPoint();
//    cvWaitKey(0);

    go(fd);
    
//
    while (true) {
        
        getNewFrame();
        locateCar();
        if (refreshPathStatus()) {
            turnToNextPoint();
            go(fd);
        }
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);

    }
    stop(fd);
    
    
    
}