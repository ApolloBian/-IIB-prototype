//
//  perspectiveTransform.hpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/3/15.
//  Copyright © 2015 Apollo. All rights reserved.
//
#ifndef image_h
#define image_h

#include <iostream>
#include "cv.h"
#include "highgui.h"

const int leftwindowX = 100;
const int leftwindowY = 100;
const int rightwindowX = 750;
const int rightwindowY = 100;
const int middlewindowX = 425;
const int middlewindowY = 100;
const int squareWindowSize = 600;
const int carSize = 30;



template <class T>
class Image {
private:
    IplImage * p_img;
    
public:
    Image(IplImage * img = nullptr) {p_img = img; }
    T * operator[](const int rowIndex) {
        return ((T*)(p_img->imageData + rowIndex * p_img->widthStep));
    }
};

struct RGBPixel {
    unsigned char r,g,b;
    RGBPixel(){r = g = b = 0; }
};

typedef Image<RGBPixel> RGBImage;           /* Use [i][j].r/g/b to visit the i row j line  */
typedef Image<unsigned char> BWImage;       /* Use [i][j] to visit the i row j line        */
double distance(CvPoint & p1 , CvPoint & p2 );

int paraLine(CvPoint * line1 , CvPoint * line2);
double length(CvPoint * l1);
double distance(CvPoint & p1 , CvPoint * l1);
double distance(CvPoint & p1 , CvPoint & l1 , CvPoint & l2);


struct carSeq {
    int currentIndex;
    int currentEnd;
    int entranceIndex;
    int entranceEnd; // 0/1
    int numberOfLines;

    CvPoint * line [100];
    bool lineStatus[100][2];
    
    carSeq();
    void build(CvSeq * lines);
    void reset();
    void simplify();

};

void print(carSeq & path, IplImage * tarImage);


#endif /* image_h */
