#include "controlling.h"
#include <iostream>
#include <cmath>
#include "timecalc.h"

using namespace cv;

extern CvMat * transmat;
extern CvSeq* lines;
extern CvPoint * lineArray[20];
extern carSeq path;
extern int fd;




int _distance = 30;
int vmin = 50;
int vmax = 256;
int smin = 80;

CvHistogram* carFrontHistogram;
CvHistogram* carBackHistogram;









//本地变量
CvCapture * cam = cvCreateCameraCapture(1);
IplImage * cameraImage;
IplImage * monitorImage = cvCreateImage(cvSize(squareWindowSize, squareWindowSize), IPL_DEPTH_8U, 3);
IplImage * hsv , * hue , * mask , * backprojectF, *backprojectB;
CvRect track_windowF = {0,0,squareWindowSize,squareWindowSize};
CvRect track_windowB = {0,0,squareWindowSize,squareWindowSize};
CvBox2D track_boxF, track_boxB;
CvConnectedComp track_compF,track_compB;
int _vmin = vmin, _vmax = vmax,_smin = smin;
CvPoint pointB, pointF;
CvPoint pointM;


void getNewFrame() {                                                                //
    cameraImage = cvQueryFrame(cam);
    cvWarpPerspective(cameraImage, monitorImage, transmat);
}

void locateCar() {                                                                                   //追踪小车，更新图像,更新小车的位置变量
    cvCvtColor(monitorImage, hsv, CV_RGB2HSV);
    cvInRangeS(hsv,cvScalar(0,_smin,MIN(_vmin,_vmax),0),cvScalar(180,256,MAX(_vmin,_vmax),0),mask);  //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
    cvSplit( hsv, hue, 0, 0, 0 );//分离H分量
    
    cvCalcBackProject(&hue, backprojectF , carFrontHistogram);
    cvAnd(backprojectF, mask, backprojectF,0);
    cvCamShift( backprojectF, track_windowF,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_compF, &track_boxF );
    track_windowF = track_compF.rect;
    cvCalcBackProject(&hue, backprojectB, carBackHistogram);
    cvAnd(backprojectB, mask, backprojectB,0);
    cvCamShift( backprojectB, track_windowB,cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),&track_compB, &track_boxB );
    track_windowB = track_compB.rect;

    pointB.x = track_windowB.x+track_windowB.width/2;
    pointB.y = track_windowB.y+track_windowB.height/2;
    pointF.x = track_windowF.x+track_windowF.width/2;
    pointF.y = track_windowF.y+track_windowF.height/2;
    pointM.x = (pointB.x +pointF.x)/2;
    pointM.y = (pointF.y + pointB.y)/2;
    
    CvRect rect,rectB,rectF;
    rect.width = rect.height = rectB.width = rectB.height = rectF.width = rectF.height = 10;
    rect.x = pointM.x-5;
    rect.y = pointM.y-5;
    rectB.x = pointB.x-5;
    rectB.y = pointB.y-5;
    rectF.x = pointF.x-5;
    rectF.y = pointF.y-5;
    cvRectangleR(monitorImage,rect, CV_RGB(255,255,255),3,CV_AA,0);
    cvRectangleR(monitorImage,rectB, CV_RGB(0,255,0),3,CV_AA,0);
    cvRectangleR(monitorImage,rectF, CV_RGB(0,0,255),3,CV_AA,0);
    cvLine(monitorImage, pointB, pointF ,CV_RGB(255,0,0),1,CV_AA,0);
    //    cvRectangleR(monitorImage,track_windowB, CV_RGB(255,0,0),3,CV_AA,0);
    //    cvShowImage("parameter", backprojectF);
    //    cvRectangleR(monitorImage,track_windowF, CV_RGB(0,0,255),3,CV_AA,0);
    //    cvRectangleR(monitorImage,track_windowB, CV_RGB(0,255,0),3,CV_AA,0);
}

int refreshPathStatus() {
    static double distanceToTarget = 1000000;
    double dist = distance(pointM, path.line[path.currentIndex][path.currentEnd]);
    if (dist <= distanceToTarget+2) {
        distanceToTarget = dist;
        return 0;
    }
    distanceToTarget = 1000000;
    if ((path.currentIndex == path.numberOfLines-1)&&path.currentEnd) {
        path.reset();
        return -1;
    }
    path.lineStatus[path.currentIndex][path.currentEnd] = true;
    if (path.currentEnd == 1) {
        path.currentEnd = 0;
        if (distance(path.line[path.currentIndex][1], path.line[path.currentIndex+1][0]) < _distance) {
            path.lineStatus[path.currentIndex+1][0] = true;
            path.currentEnd = 1;
        }
        ++path.currentIndex;
    } else {
        path.lineStatus[path.currentIndex][0] = true;
        path.currentEnd = 1;
    }
    return 1;
}

void turnToNextPoint() {
    int stoptime = 100;
    printf("turn");
    stop(fd);
    cvWaitKey(stoptime);
    getNewFrame();
    locateCar();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);

    double x1 = pointF.x-pointB.x;
    double y1 = pointF.y-pointB.y;
    double x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
    double y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
    double theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
    double degree = acos(theta)*180/M_PI;
    do {
        double time = degree*14;
        if (y1*x2-y2*x1 < 0) {
            turnright(fd);
        } else {
            turnleft(fd);
        }
        cvWaitKey(time);
        stop(fd);
        cvWaitKey(stoptime);
        getNewFrame();
        locateCar();
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);
        x1 = pointF.x-pointB.x;
        y1 = pointF.y-pointB.y;
        x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
        y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
        theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
        degree = acos(theta)*180/M_PI;
    } while (degree > 5);
    stop(fd);
}

int gotoNextPoint() {
    printf("go\n");
    go(fd);
    getNewFrame();
    locateCar();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);

    int status = refreshPathStatus();
    while (status==0) {
        getNewFrame();
        locateCar();
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);

        status = refreshPathStatus();
        if (cvWaitKey(1) == '\r') {
            stop(fd);
            return -1;
        }
    }
    stop(fd);
    return 1;
}

void controlling(int fd) {
    // 地图目标点
    // 图像追踪部分的变量
    hsv = cvCreateImage(cvGetSize(monitorImage), IPL_DEPTH_8U, 3);
    hue = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    mask = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectF = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backprojectB = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    
//    cvNamedWindow("parameter");
//    cvCreateTrackbar("vmin", "parameter", &vmin,300);
//    cvCreateTrackbar("vmax", "parameter", &vmax, 300);
//    cvCreateTrackbar("smin", "parameter", &smin, 300);
    
    

    calcHistogram(carfront,carFrontHistogram);
    calcHistogram(carback, carBackHistogram);
    cvNamedWindow(monitor);
    cvMoveWindow(monitor, leftwindowX, leftwindowY);
    
    
//试验前调整，防止追踪不到
    while (true) {
        getNewFrame();
        locateCar();
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);
        if (cvWaitKey(33) == '\r') {
            break; 
        }
    }
    path.currentEnd = 1;
    path.currentIndex = 0;
    path.lineStatus[0][0] = true;
    
//开始
    turnToNextPoint();
    while (gotoNextPoint()!=-1) {
        turnToNextPoint();
    }
}
//int refreshPathStatus(){
//    for (int i = 0 ; i < path.numberOfLines; ++i) {
//        for (int j = 0 ; j < 2 ; ++j) {
//            if (path.lineStatus[i][j]) {
//                continue;
//            }
//            if (distance(pointM, path.line[i][j]) < _distance) {
//                path.lineStatus[i][j] = true;
//            }
//        }
//    }
//    if (path.lineStatus[path.currentIndex][path.currentEnd]) {
//        double minTurn = -1;
//        double curTurn = 0;
//        double x1 = pointF.x-pointB.x;
//        double y1 = pointF.y-pointB.y;
//        double x2 = 1;
//        double y2 = 1;
//        bool flag = false;
//        for (int i = 0 ; i < path.numberOfLines; ++i) {
//            for (int j = 0 ; j < 2 ; ++j) {
//                if (path.lineStatus[i][j]) {
//                    continue;
//                }
//                x2 = path.line[i][j].x-pointM.x;
//                y2 = path.line[i][j].y-pointM.y;
//                curTurn = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
//                if (curTurn > minTurn) {
//                    flag = true;
//                    minTurn = curTurn;
//                    path.currentIndex = i;
//                    path.currentEnd = j;
//                }
//            }
//        }
//        if (!flag) {
//            return -1;
//        }
//        return 1;
//    }
//    return 0;
//}


//void gotoEntrance() {
//    path.reset();
//    getNewFrame();
//    locateCar();
//    refreshPathStatus();
//    _distance = 30;
//    if (path.lineStatus[path.entranceIndex][path.entranceEnd]) {
//        path.currentEnd = !path.currentEnd;
//        gotoNextPoint();
//        turnToNextPoint();
//        return;
//    }
//
//    turnToNextPoint();
//    gotoNextPoint();
//    path.reset();
//    path.currentEnd = !path.currentEnd;
//    turnToNextPoint();
//}

//int gotoNextPoint() {
//    printf("go");
//    go(fd);
//
//    double x1 = pointF.x-pointB.x;
//    double y1 = pointF.y-pointB.y;
//    double x2 = 1;
//    double y2 = 1;
//    double theta = -1;
//    x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
//    y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
//    theta = x1*x2+y1*y2;
//
//
//    while (theta > 0) {
//        getNewFrame();
//        locateCar();
//        x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
//        y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
//        x1 = pointF.x-pointB.x;
//        y1 = pointF.y-pointB.y;
//        theta = x1*x2+y1*y2;
//
//
//        print(path, monitorImage);
//        cvShowImage(monitor, monitorImage);
//
//        // debug
////        turnToNextPoint();
//
//        //
//
//
//        if (refreshPathStatus()!=0) {
//            return refreshPathStatus();
//        }
//    }
//
//    path.lineStatus[path.currentIndex][path.currentEnd] = true;
//
//    return refreshPathStatus();
//}



/*
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
 bool flag = false;
 if (y1*x2-y2*x1 < 0) {
 turnright(fd);
 flag = true;
 } else {
 turnleft(fd);
 }
 double COS = -1;
 while (theta<cos(M_PI/18)||theta >= COS) {
 COS = theta;
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
 
 stop(fd);
 cvWaitKey(100);
 int time;
 getNewFrame();
 locateCar();
 x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
 y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
 x1 = pointF.x-pointB.x;
 y1 = pointF.y-pointB.y;
 theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
 time = acos(theta)*180/M_PI*16;
 //4.4s
 if (flag) {
 turnleft(fd);
 } else {
 turnright(fd);
 }
 cvWaitKey(time);
 
 printf("cos = %f",acos(theta)*180/M_PI);
 //    (fd);
 //
 //    go(fd);
 //
 getNewFrame();
 locateCar();
 }
 
 
 */