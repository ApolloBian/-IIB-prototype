//
//  tracking.cpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/7/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#include "tracking.h"

const char monitor[] = "monitor";
const char trackwindowName[] = "tracking";
extern CvMat * transmat;

CvHistogram* tarHistogram;
int vmin = 10;
int vmax = 256;
int smin = 30;

void tracking() {
    calcHistogram();
    cvNamedWindow(monitor);
    cvMoveWindow(monitor, leftwindowX, leftwindowY);
    cvNamedWindow(trackwindowName);
    cvMoveWindow(trackwindowName, rightwindowX, rightwindowY);
    cvCreateTrackbar( "Vmin", trackwindowName, &vmin, 256, 0 );
    cvCreateTrackbar( "Vmax", trackwindowName, &vmax, 256, 0 );
    cvCreateTrackbar( "Smin", trackwindowName, &smin, 256, 0 );
    
    
    IplImage * cameraImage;
    IplImage * monitorImage;
    IplImage * hsv , * hue , * mask , * backproject;
    CvRect track_window = {0,0,squareWindowSize,squareWindowSize};
    CvBox2D track_box;
    CvConnectedComp track_comp;
    
    
    monitorImage = cvCreateImage(cvSize(squareWindowSize, squareWindowSize), IPL_DEPTH_8U, 3);
    hsv = cvCreateImage(cvGetSize(monitorImage), IPL_DEPTH_8U, 3);
    hue = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    mask = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    backproject = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 1);
    CvCapture * cam = cvCreateCameraCapture(-1);
    while (true) {
        cameraImage = cvQueryFrame(cam);
        cvWarpPerspective(cameraImage, monitorImage, transmat);
        cvCvtColor(monitorImage, hsv, CV_RGB2HSV);
        int _vmin = vmin, _vmax = vmax;
        
        cvInRangeS( hsv, cvScalar(0,smin,MIN(_vmin,_vmax),0),
                   cvScalar(180,256,MAX(_vmin,_vmax),0), mask );                                                               //制作掩膜板，只处理像素值为H：0~180，S：smin~256，V：vmin~vmax之间的部分
        cvSplit( hsv, hue, 0, 0, 0 );                                                                                          //分离H分量
        cvCalcBackProject(&hue, backproject , tarHistogram);
        cvAnd(backproject, mask, backproject,0);
        cvCamShift( backproject, track_window,
                   cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
                   &track_comp, &track_box );                                                                                   //使用MeanShift算法对backproject中的内容进行搜索,返回跟踪结果
        track_window = track_comp.rect;
//        cvEllipseBox( monitorImage, track_box, CV_RGB(255,0,0), 3, CV_AA, 0 );
        cvRectangleR(monitorImage,track_window, CV_RGB(255,0,0),3,CV_AA,0);
        cvShowImage(monitor, monitorImage);
        cvShowImage(trackwindowName, backproject);
        if (cvWaitKey(1) == '\r') {
            break;
        }
    }
    
    
}




void calcHistogram() {
    IplImage* tar = cvLoadImage("/Users/apollo/Desktop/target.png"); //目标图像
    IplImage* tarhsv = cvCreateImage(cvGetSize(tar), 8, 3); //HSV空间的目标图像
    IplImage* tarhue = cvCreateImage(cvGetSize(tar), 8, 1); //目标的色度通道
    cvCvtColor(tar, tarhsv, CV_BGR2HSV); //目标从RGB空间转换至HSV空间
    cvSplit(tarhsv, tarhue, NULL, NULL, NULL); //只提取目标的色度通道
    int bins = 30; //指定直方图的精细程度(横轴等分为多少区间)
    float range[] = {0, 180}; //指定直方图横轴的取值范围,色度通道最大值就是180
    float* ranges[] = {range}; //因为cvCreateHist的参数必须是float**
    tarHistogram = cvCreateHist(1, &bins, CV_HIST_ARRAY, ranges);//创建直方图
    cvCalcHist(&tarhue, tarHistogram); //计算目标的色度通道的直方图

}