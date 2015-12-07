
//
//  tracking.cpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/7/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#include "tracking.h"






void calcHistogram(const char * tarPath,CvHistogram *& tarHistogram) {
    IplImage* tar = cvLoadImage(tarPath); //目标图像
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