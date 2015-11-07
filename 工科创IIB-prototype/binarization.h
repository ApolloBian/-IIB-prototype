//
//  binarization.h
//  工科创IIB-prototype
//
//  Created by Apollo on 11/3/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#ifndef binarization_h
#define binarization_h
#include "image.h"

void bin();
void thin();

void binaryzation(int pos);
//void binerode(int pos);
void thin(int pos);

void cvThin(IplImage * src, IplImage * dst , int iterations = 20);

#endif /* binarization_hpp */
