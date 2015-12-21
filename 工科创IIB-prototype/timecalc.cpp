#include "timecalc.h"
#include <iostream>

using namespace std;


timeval a,b;
bool flag = false;
timeval* cur;

void myTime::reset() {
    flag = false;
}

void myTime::stopwatch() {
    if (flag) cur = &b;
    else cur = &a;
    flag = !flag;
    gettimeofday(cur, NULL);
}

void myTime::showTime() {
  
    long t = (b.tv_sec - a.tv_sec)*1000000 + b.tv_usec -a.tv_usec;
    long s = t/1000000;
    long ms = (t-1000000*s)/1000;
        
    printf("%lds %ldms",s,ms);
    
}