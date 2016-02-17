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




int _distance = 40;
int vmin = 50;
int vmax = 256;
int smin = 80;

int movingFrames = 4;
int staticFrames = 7;

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

}

int refreshPathStatus() {
    static double distanceToTarget = 1000000;
    double dist = distance(pointM, path.line[path.currentIndex][path.currentEnd]);
    if (dist <= distanceToTarget+1) {
        distanceToTarget = dist;
        return 0;
    }
    distanceToTarget = 1000000;
    if ((path.currentIndex == path.numberOfLines-1)&&path.currentEnd) {
//        if (path.lineStatus[path.currentIndex][0]&&path.lineStatus[path.currentIndex][1]) {
//            return -2;
//        }
        return -2;
    }
   //_________
    if (dist > 50) {
        return 1;
    }
    //________
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
}//instantly

void flushInfomation() {
    getNewFrame();
    locateCar();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);
}//45 ms in vague average
//22.5度大约刷新4帧



double calculateDegree() {
    double x1 = pointF.x-pointB.x;
    double y1 = pointF.y-pointB.y;
    double x2 = path.line[path.currentIndex][path.currentEnd].x-pointM.x;
    double y2 = path.line[path.currentIndex][path.currentEnd].y-pointM.y;
    double theta = (x1*x2+y1*y2)/sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2));
    double degree = acos(theta)*180/M_PI;
    if (y1*x2-y2*x1 < 0) {
        return -degree;
    } else {
        return degree;
    }
}

// originallt *6
int multratio = 11;
void turnLeftDegree(int degree) {
//    if (degree > 70) {
//        turnLeftDegree(45);
//        turnLeftDegree(degree-45);
//    }
    
    int times = degree*multratio/45;
    turnleft(fd);
    for (int i = 0 ; i < times ; ++i) {
        flushInfomation();
        calculateDegree();
        if (degree < 6 && degree > -6) {
            break;
        }
    }
    stop(fd);
    for (int i = 0 ; i < 3 ; ++i) {
        flushInfomation();
    }
    calculateDegree();


}

void turnRightDegree(int degree) {
//    if (degree > 70 ) {
//        turnRightDegree(45);
//        turnRightDegree(degree-45);
//    }
    
    int times = degree*multratio/45;
    turnright(fd);
    for (int i = 0 ; i < times ; ++i) {
        flushInfomation();
        calculateDegree();
        if (degree < 6 && degree > -6) {
            break;
        }
    }
    stop(fd);
    for (int i = 0 ; i < 3 ; ++i) {
        flushInfomation();
    }
    calculateDegree();
}






void turnToNextPoint() {
    int stoptime = 100;
    stop(fd);
    cvWaitKey(stoptime);
    getNewFrame();
    locateCar();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);

    double degree = calculateDegree();
    int count = 0;
    double a;
    do {
        ++count;
        if (degree < 0) turnRightDegree(-degree);
        else turnLeftDegree(degree);
        flushInfomation();
        degree = calculateDegree();
        a  = degree;
        if (a < 0) {
            a = -a;
        }
    } while ((a > 3 && count < 5));
    
    //||(count >= 5 && a > 6)
    std::cout<<degree<<std::endl;
    stop(fd);
}

int gotoNextPoint() {
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
//        if (cvWaitKey(1) == '\r') {
//            stop(fd);
//            return -1;
//        }
    }
    if (status == -2) {
        stop(fd);
        return -1;
    }
    stop(fd);
    return 1;
}

void controlling(int fd) {
    track_windowF = {0,0,squareWindowSize,squareWindowSize};
    track_windowB = {0,0,squareWindowSize,squareWindowSize};
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
        print(path, monitorImage);
        cvShowImage(monitor, monitorImage);
        if (cvWaitKey(33) == '\r') {
            break; 
        }
    }
    path.currentEnd = 1;
    path.currentIndex = 0;
//    path.lineStatus[0][0] = true;
    
//开始
    turnToNextPoint();
    while (gotoNextPoint()!=-1) {
        turnToNextPoint();
    }
}


