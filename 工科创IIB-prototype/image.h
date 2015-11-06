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


#endif /* image_h */
