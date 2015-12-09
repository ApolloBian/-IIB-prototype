#include "image.h"
#include <cmath>

using namespace std;

double theta = M_PI/8;
double ratio = 20;

double length(CvPoint & p1 , CvPoint & p2 ) {
    return sqrt(pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2));
}

double length(CvPoint * l1) {
    return length(l1[0],l1[1]);
}

double distance(CvPoint & p1 , CvPoint * l1) {
    double x1 = p1.x-l1[0].x;
    double y1 = p1.y-l1[0].y;
    double x2 = l1[1].x-l1[0].x;
    double y2 = l1[1].y-l1[0].y;
    double l = (x1*x2+y1*y2)/length(l1[0], l1[1]);
    return sqrt(x1*x1+y1*y1-l*l);
}


struct index {
    int a;
    int b;
    void set(int z , int x) {
        a = z;
        b = x;
    }
};



void carSeq::build(CvSeq * lines) {
    int maxSum = 0;
    numberOfLines = lines->total;
    for (int i = 0 ; i < lines->total; ++i) {
        line[i] = (CvPoint * )cvGetSeqElem(lines , i );
        if (line[i][1].x+line[i][1].y > maxSum) {
            maxSum = line[i][1].x+line[i][1].y;
            entranceIndex = i;
            entranceEnd = 1;
        }
        if (line[i][0].x+line[i][0].y > maxSum) {
            maxSum = line[i][0].x+line[i][0].y;
            entranceIndex = i;
            entranceEnd = 0;
        }
    }
    currentEnd = entranceEnd;
    currentIndex = entranceIndex;
}

carSeq::carSeq() {
    reset();
}

void carSeq::reset() {
    for (int i = 0 ; i < 100 ; ++i) {
        lineStatus[i][0] = lineStatus[i][1] = false;
    }
    currentEnd = entranceEnd;
    currentIndex = entranceIndex;
}

void print(carSeq & path, IplImage * tarImage) {
    CvRect rect;
    rect.width = 10;
    rect.height = 10;
    CvScalar color[] = {CV_RGB(255,0,0),CV_RGB(0,255,0)};
    
    for (int i = 0 ; i < path.numberOfLines; ++i) {
        for (int j = 0 ; j < 2 ; ++j) {
            CvPoint * line = path.line[i];
            rect.x = line[0].x-5;
            rect.y = line[0].y-5;
            cvRectangleR(tarImage,rect, color[path.lineStatus[i][j]],3,CV_AA,0);
        }
        
        //        rect.x = line[1].x-5;
        //        rect.y = line[1].y-5;
        //        cvRectangleR(tarImage,rect, CV_RGB(255,0,0),3,CV_AA,0);
    }
    CvPoint * line;
//    line = path.line[path.entranceIndex];
//    rect.x = line[path.entranceEnd].x-5;
//    rect.y = line[path.entranceEnd].y-5;
//    cvRectangleR(tarImage,rect, CV_RGB(0,0,255),3,CV_AA,0);
    
    line = path.line[path.currentIndex];
    rect.x = line[path.currentEnd].x-5;
    rect.y = line[path.currentEnd].y-5;
    cvRectangleR(tarImage,rect, CV_RGB(255,255,255),3,CV_AA,0);
}











//bool sameLine(CvPoint * line1 , CvPoint * line2) {                                  //如果line1和line2是同一条直线，将其合并并且返回true，合并的方式是修改line1
//    CvPoint points[4] = {line1[0],line1[1],line2[0],line2[1]};
////    double k1 = (line1[1].y-line1[0].y)/(line1[1].x-line1[0].x);
////    double k2 = (line2[1].y-line2[0].y)/(line2[1].x-line2[0].x);
//
//    double x1 = line1[1].x-line1[0].x;
//    double y1 = line1[1].y-line1[0].y;
//    double x2 = line2[1].x-line2[0].x;
//    double y2 = line2[1].y-line2[0].y;
//    
//    
//    double c = (x1*x2-y1*y2)/(sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2)));
//    
//    if (c < cos(theta)) {
//        
//        return false;
//    }
//    struct index max_i , min_i;
//    
//    double max_d = 0;
//    double min_d = 100000;
//    double d;
//    
//    for (int i = 0 ; i < 3 ; ++i ) {
//        for (int j =  i+1; j < 4 ; ++j ) {
//            d = length(points[i], points[j]);
//            if (d > max_d) {
//                max_d = d;
//                max_i.set(i, j);
//                continue;
//            }
//            if (d < min_d) {
//                min_d = d;
//                min_i.set(i,j);
//            }
//        }
//    }
////    if (abs(min_i.a-min_i.b)==1 &&min_i.a*min_i.b!=2 &&abs(max_i.a-max_i.b)!=1) {
////        return false;
////    }
//    CvPoint mid1,mid2;
//    mid1.x = line1[1].x/2+line1[0].x/2;
//    mid1.y = line1[1].y/2+line1[0].y/2;
//    mid2.x = line2[1].x/2+line2[0].x/2;
//    mid2.y = line2[1].y/2+line2[0].y/2;
//    
//    double rat1 = MAX(length(line1[0], line1[1]),length(line2[0], line2[1]))/length(mid1,mid2);
//    double rat2 = (MIN(distance(line1[0],line2), distance(line2[0],line1)))/(MAX(length(line1), length(line2)));
//    cout<<rat2<<endl;
//    if (rat2 < 1 ) {
//        line1[0] = points[max_i.a];
//        line1[1] = points[max_i.b];
//        return true;
//    }
//    
//    
//    
////    if (max_d / min_d > 5 ) {
////        line1[0] = points[max_i.a];
////        line1[1] = points[max_i.b];
////        return true;
////    }
//    return false;
//}