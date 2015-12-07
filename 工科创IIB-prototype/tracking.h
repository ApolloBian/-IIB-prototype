//
//  tracking.hpp
//  工科创IIB-prototype
//
//  Created by Apollo on 11/7/15.
//  Copyright © 2015 Apollo. All rights reserved.
//

#ifndef tracking_h
#define tracking_h
#include "image.h"

const char monitor[] = "monitor";
const char trackwindowName[] = "tracking";
const char carfront[] = "/Users/apollo/Developer/PCControlled_Car_II-B/主程序/工科创IIB-prototype/工科创IIB-prototype/IMG_1033.JPG";
const char carback[] =  "/Users/apollo/Developer/PCControlled_Car_II-B/主程序/工科创IIB-prototype/工科创IIB-prototype/IMG_1034.JPG";


void controlling();
void calcHistogram(const char * tarPath,CvHistogram * &tarHistogram);




#endif /* tracking_hpp */
