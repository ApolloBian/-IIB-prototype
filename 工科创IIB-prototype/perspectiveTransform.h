//
//  perspectiveTransform.hpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/3/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#ifndef perspectiveTransform_hpp
#define perspectiveTransform_hpp
#include "image.h"





void perspective_mouse(int mouseAction , int x , int y , int flags , void * param);        //视角变换鼠标回调函数

void setVariables();

void setPoints();

#endif /* perspectiveTransform_hpp */

