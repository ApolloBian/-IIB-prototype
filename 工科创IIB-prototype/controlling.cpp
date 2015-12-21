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

int movingFrames = 4;
int staticFrames = 7;

CvHistogram* carFrontHistogram;
CvHistogram* carBackHistogram;









//本地变量
CvCapture * cam = cvCreateCameraCapture(0);
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
    if (dist <= distanceToTarget+3) {
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
}//instantly

void flushInfomation() {
    getNewFrame();
    locateCar();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);
}//45 ms in vague average
//22.5度大约刷新4帧

//void customWait(int time) {
//    static const double timeConstant = 22.5*8.5;
//    int frames = 4;
//    int frames = time/timeConstant;
//    if (frames < 2) {
//        getNewFrame();
//        locateCar();
//        print(path, monitorImage);
//        cvShowImage(monitor, monitorImage);
//        cvWaitKey(time);
//    } else {
//        for (int i = 0 ; i < frames ; ++i) {
//            getNewFrame();
//            locateCar();
//            print(path, monitorImage);
//            cvShowImage(monitor, monitorImage);
//            cvWaitKey(timeConstant);
//        }
//    }
//}


void turnLeftDegree(int degree) {
    int times = (degree+11)/22;

    for (int i = 0 ; i < times; ++i) {
        turnleft(fd);
        int j;
        for (j = 0 ; j < movingFrames; ++j) {
            flushInfomation();
        }
        stop(fd);
        for (j = 0 ; j < staticFrames; ++j) {
            flushInfomation();
        }
    }
    
}

void turnRightDegree(int degree) {
    int times = (degree+11)/22;
    for (int i = 0 ; i < times; ++i) {
        turnright(fd);
        int j;
        for (j = 0 ; j < movingFrames; ++j) {
            flushInfomation();
        }
        stop(fd);
        for (j = 0 ; j < staticFrames; ++j) {
            flushInfomation();
        }
    }

}


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

void turnToNextPoint() {
    int stoptime = 100;
    printf("turn");
    stop(fd);
    cvWaitKey(stoptime);
    getNewFrame();
    locateCar();
    print(path, monitorImage);
    cvShowImage(monitor, monitorImage);

    double degree = calculateDegree();
    do {
        if (degree < 0) turnRightDegree(-degree);
        else turnLeftDegree(degree);
        degree = calculateDegree();
    } while (degree > 12);
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
    track_windowF = {0,0,squareWindowSize,squareWindowSize};
    track_windowB = {0,0,squareWindowSize,squareWindowSize};
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
//    for (int j = 0 ; j < 10 ; ++j) {
//        myTime::reset();
//        myTime::stopwatch();
//        int i = 0;
//        while (i < 100) {
//            flushInfomation();
//            ++i;
//        }
//        myTime::stopwatch();
//        myTime::showTime();
//    }

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

//void turnLeftDegree(int degree) {
//    int stopTime = 100;
//    if (degree >= 30 && degree <= 60) {
//        degree = 45;
//    }
//    
//    if (degree > 60 && degree < 105) {
//        degree = 90;
//    }
//    
//    if (degree == 90) {
//        for (int i = 0 ; i < 4 ; ++i) {
//            turnleft(fd);
//            customWait(22.5*8.2);
//            stop(fd);
//            customWait(stopTime);
//        }
//        return;
//    }
//    if (degree == 45) {
//        for (int i = 0 ; i < 2 ; ++i) {
//            turnleft(fd);
//            customWait(22.5*8.2);
//            stop(fd);
//            customWait(stopTime);
//        }
//        return;
//    }
//    turnleft(fd);
//    customWait(degree*8.2);
//    stop(fd);
//    customWait(stopTime);
//    
//}
//
//void turnRightDegree(int degree) {
//    if (degree < 5) {
//        return;
//    }
//    
//    
//    int stopTime = 100;
//    
//    if (degree >= 30 && degree <= 60) {
//        degree = 45;
//    }
//    
//    if (degree > 60 && degree < 105) {
//        degree = 90;
//    }
//    
//    if (degree == 90) {
//        for (int i = 0 ; i < 4 ; ++i) {
//            turnright(fd);
//            customWait(22.5*8.2);
//            stop(fd);
//            customWait(stopTime);
//        }
//        return;
//    }
//    if (degree == 45) {
//        for (int i = 0 ; i < 2 ; ++i) {
//            turnright(fd);
//            customWait(22.5*8.2);
//            stop(fd);
//            customWait(stopTime);
//        }
//        return;
//    }
//    turnright(fd);
//    customWait(degree*8.2);
//    stop(fd);
//    customWait(stopTime);
//    
//}
