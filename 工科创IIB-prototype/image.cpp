

#include "image.h"
#include <cmath>

using namespace std;

double theta = M_PI/8;
double max_distance = 80;
struct index {
    int a;
    int b;
    void set(int z , int x) {
        a = z;
        b = x;
    }
};
int det(int a , int b , int c , int d) {
    return a*d - b*c;
}

double distance(CvPoint & p1 , CvPoint & p2) {
    return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}
double length(CvPoint * l1) {
    return distance(l1[0],l1[1]);
}
double distance(CvPoint & p1 , CvPoint * l1) {
    double x1 = p1.x-l1[0].x;
    double y1 = p1.y-l1[0].y;
    double x2 = l1[1].x-l1[0].x;
    double y2 = l1[1].y-l1[0].y;
    double l = (x1*x2+y1*y2)/sqrt(x2*x2+y2*y2);
    return sqrt(x1*x1+y1*y1-l*l);
}
double distance(CvPoint & p1 , CvPoint & l1 , CvPoint & l2) {
    double x1 = p1.x-l1.x;
    double y1 = p1.y-l1.y;
    double x2 = l2.x-l1.x;
    double y2 = l2.y-l1.y;
    double l = (x1*x2+y1*y2)/distance(l1, l2);
    return sqrt(x1*x1+y1*y1-l*l);
}



void print(carSeq & path, IplImage * tarImage) {
    CvRect rect;
    rect.width = 10;
    rect.height = 10;
    CvScalar color[] = {CV_RGB(255,0,0),CV_RGB(0,255,0)};
    for (int i = 0 ; i < path.numberOfLines; ++i) {
        for (int j = 0 ; j < 2 ; ++j) {
            CvPoint * line = path.line[i];
            rect.x = line[j].x-5;
            rect.y = line[j].y-5;
            cvRectangleR(tarImage,rect, color[path.lineStatus[i][j]],3,CV_AA,0);
//            cvRectangleR(tarImage,rect, color[path.lineStatus[i][j]],3,CV_AA,0);
//            cvRectangleR(tarImage, rect, CV_RGB(20*i, 255-20*i, 0),3,CV_AA,0);
        }
    }

    
    for (int i = 0 ; i < path.numberOfLines-1; ++i) {
        cvLine(tarImage, path.line[i][1], path.line[i+1][0], cvScalar(255));
    }
    for(int i = 0; i < path.numberOfLines; i++) {
        CvPoint* line = path.line[i];
        cvLine( tarImage, line[0], line[1], cvScalar(255));
    }
    
    CvPoint * line;
    line = path.line[path.currentIndex];
    rect.x = line[path.currentEnd].x-5;
    rect.y = line[path.currentEnd].y-5;
    cvRectangleR(tarImage,rect, CV_RGB(255,255,255),3,CV_AA,0);
}



void reverseLine(CvPoint * line) {
    swap(line[0], line[1]);
}

void setJoint(CvPoint * line1 , CvPoint * line2) {
    struct index ind;
    int multdist = 1000000;
    //交点
    for (int i = 0 ; i < 2; ++i) {
        for (int j = 0 ; j < 2 ; ++j) {
            double dist = distance(line1[i],line2[j]);
            if (multdist > dist) {
                multdist = dist;
                ind.set(i, j);
            }
        }
    }
    //交点设置为前一条线的1和后一条线的0；
    if (!ind.a) {
        reverseLine(line1);
    }
    if (ind.b) {
        reverseLine(line2);
    }
}

int paraLine(CvPoint * line1 , CvPoint * line2) {                                  //如果line1和line2是同一条直线，将其合并并且返回true，合并的方式是修改line1
    
    // 如何合并直线？夹角超过30度的直线可以不用考虑（cos），点到直线的距离，阈值；
    CvPoint points[4] = {line1[0],line1[1],line2[0],line2[1]};
    
    double x1 = line1[1].x-line1[0].x;
    double y1 = line1[1].y-line1[0].y;
    double x2 = line2[1].x-line2[0].x;
    double y2 = line2[1].y-line2[0].y;                                              //两个方向向量
    
    struct index max_i;
    double max_d = 0;
   
    
    double c = (x1*x2+y1*y2)/(sqrt((x1*x1+y1*y1)*(x2*x2+y2*y2)));                   //calculate cosθ
    if (abs(c) < cos(theta)) {
        return false;
    }
    
    double d;
    for (int i = 0 ; i < 3 ; ++i ) {
        for (int j =  i+1; j < 4 ; ++j ) {
            d = distance(points[i], points[j]);
            if (d > max_d) { max_d = d;max_i.set(i, j);}
        }
    }
    double dist = distance(line1[0], line2);
    printf("same line distance = %f",dist);

    if (dist < max_distance ) {
        line1[0] = points[max_i.a];
        line1[1] = points[max_i.b];
        printf("return -1\n");
        return -1;
    }
    printf("return 1\n");
    return 1;
}

double minEndDistance(CvPoint * line1 , CvPoint * line2) {
    double min_d = 10000000;
    double d;
    for (int i = 0 ; i < 2; ++i) {
        for (int j = 0 ; j < 2; ++j) {
            d = distance(line1[i], line2[j]);
            if (min_d > d) {
                min_d = d;
            }
        }
    }
    return min_d;
}


void carSeq::build(CvSeq * lines) {
    int entranceIndex , entranceEnd;
    entranceIndex = entranceEnd = 0;
    int maxSum = 0;
    numberOfLines = lines->total;
    printf("num of ls before build: %d \n",numberOfLines);
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
    swap(line[0], line[entranceIndex]);
    int pos = 0;
    int i;
    double min_d = 1000000;
    int nextLineIndex;

    while (pos < numberOfLines) {
        nextLineIndex = pos+1;
        min_d = 1000000;
        for (i = pos+1; i < numberOfLines; ++i) {
            int status = paraLine(line[pos], line[i]);
            if (status == -1) {
                for (int j = i+1 ; j < numberOfLines ; ++j) line[j-1] = line[j];
                --i;
                --numberOfLines;
                
            }
            if (status == 0) {
                double d = minEndDistance(line[pos], line[i]);
                if (d < min_d) {
                    min_d = d;
                    nextLineIndex = i;
                }
            }
        }
        swap(line[pos+1], line[nextLineIndex]);
        setJoint(line[pos], line[pos+1]);
        ++pos;
    }
    printf("number Of Lines = %d \n",numberOfLines);
    currentEnd = 0;
    currentIndex = 0;
}

carSeq::carSeq() {
    reset();
}

void carSeq::reset() {
    for (int i = 0 ; i < 100 ; ++i) {
        lineStatus[i][0] = lineStatus[i][1] = false;
    }
    currentEnd = 0;
    currentIndex = 0;
}

