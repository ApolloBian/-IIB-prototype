#include "controlling.h"
#include <iostream>
#include <cmath>
#include "timecalc.h"

using namespace cv;

/* debug section

 timeval a,b;

 
 gettimeofday(&b, NULL);
 myTime::timeCalc(a, b);
 
 
 gettimeofday(&b, NULL);
 myTime::timeCalc(a, b);
 
 gettimeofday(&a, NULL);

 
 */

extern CvMat * transmat;
extern CvSeq* lines;
extern CvPoint * lineArray[20];
extern carSeq path;

extern int fd;




double COS  = cos(M_PI/60);
int _distance = 80;

int vmin = 50;
int vmax = 256;
int smin = 80;
//int vmin = 50;
//int vmax = 256;
//int smin = 80;


CvHistogram* carFrontHistogram;
CvHistogram* carBackHistogram;









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
CvPoint pointM;


void getNewFrame() {                                                                //
    cameraImage = cvQueryFrame(cam);
    cvWarpPerspective(cameraImage, monitorImage, transmat);
}

void locateCar() {                                                                  //追踪小车，更新图像,更新小车的位置变量
    cvCvtColor(monitorImage, hsv, CV_RGB2HSV);
    cvInRangeS(hsv,cvScalar(0,_smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0),mask);                                                               //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
    cvSplit( hsv, hue, 0, 0, 0 );//分离H分量
    
    
    cvCalcBackProject(&hue, backprojectF , carFrontHistogram);
    cvShowImage("parameter", backprojectF);

    cvAnd(backprojectF, mask, backprojectF,0);
    cvCamShift( backprojectF, track_windowF,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_compF, &track_boxF );
    track_windowF = track_compF.rect;
    cvRectangleR(monitorImage,track_windowF, CV_RGB(0,0,255),3,CV_AA,0);

    cvCalcBackProject(&hue, backprojectB, carBackHistogram);
    cvAnd(backprojectB, mask, backprojectB,0);
    cvCamShift( backprojectB, track_windowB,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_compB, &track_boxB );
    track_windowB = track_compB.rect;
    cvRectangleR(monitorImage,track_windowB, CV_RGB(0,255,0),3,CV_AA,0);
    
    
    
    pointB.x = track_windowB.x+track_windowB.width/2;
    pointB.y = track_windowB.y+track_windowB.height/2;
    
    pointF.x = track_windowF.x+track_windowF.width/2;
    pointF.y = track_windowF.y+track_windowF.height/2;
    
    pointM.x = (pointB.x +pointF.x)/2;
    pointM.y = (pointF.y + pointB.y)/2;
    
//    cvRectangleR(monitorImage,track_windowB, CV_RGB(255,0,0),3,CV_AA,0);
    CvRect rect;
    rect.width = rect.height = 10;
    rect.x = pointM.x-5;
    rect.y = pointM.y-5;
    cvRectangleR(monitorImage,rect, CV_RGB(255,255,255),3,CV_AA,0);
    
    
    cvLine(monitorImage, pointB, pointF ,CV_RGB(255,0,0),1,CV_AA,0);
    
//    printf("(%d,%d)",pointM.x,pointM.y);
    
    
   
}


int refreshPathStatus(){
    for (int i = 0 ; i < path.numberOfLines; ++i) {
        for (int j = 0 ; j < 2 ; ++j) {
            if (path.lineStatus[i][j]) {
                continue;
            }
            if (length(pointM, path.line[i][j]) < _distance) {
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
                x2 = path.line[i][j].x-pointM.x;
                y2 = path.line[i][j].y-pointM.y;
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
            return -1;
        }
        return 1;
    }
    return 0;
}


void turnToNextPoint() {
    printf("turn");
    double x1 = pointF.x-pointB.x;
    double y1 = pointF.y-pointB.y;
    double x2 = 1;
    double y2 = 1;
    double theta = -1;
    x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
    y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
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
        x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
        y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
        x1 = pointF.x-pointB.x;
        y1 = pointF.y-pointB.y;
        theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
    }
    printf("cos = %f",theta);
    stop(fd);
    getNewFrame();
    locateCar();
}

int gotoNextPoint() {
    printf("go");
    go(fd);
    while (true) {
        getNewFrame();
        locateCar();
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);
        
        // debug
            
        //
        
        
        if (refreshPathStatus()!=0) {
            return refreshPathStatus();
        }
    }
    return 0;
}




void gotoEntrance() {
    path.reset();
    turnToNextPoint();
    gotoNextPoint();
    path.reset();
    path.currentEnd = !path.currentEnd;
}



void controlling(int fd) {
    // 地图目标点
    // 图像追踪部分的变量
    hsv = cvCreateImage(cvGetSize(monitorImage), IPL_DEPTH_8U, 3);
    hue = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    mask = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectF = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectB = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    
    cvNamedWindow("parameter");
    cvCreateTrackbar("vmin", "parameter", &vmin,300);
    cvCreateTrackbar("vmax", "parameter", &vmax, 300);
    cvCreateTrackbar("smin", "parameter", &smin, 300);
    
    

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

    while (true) {
        gotoEntrance();
        while (gotoNextPoint()!=-1) {
            turnToNextPoint();
        }
    }
    
    
//    getNewFrame();
//    locateCar();
//    refreshPathStatus();
//    print(path, monitorImage);
//    cvShowImage(monitor, monitorImage);
//
//    go(fd);
//    
////
//    while (true) {
//        getNewFrame();
//        locateCar();
//        
//        
//        if (refreshPathStatus()) {
//            turnToNextPoint();
//            go(fd);
//        }
//        print(path, monitorImage);
//        cvShowImage(monitor, monitorImage);
//    }

}