#include <iostream>
#include <time.h>
#include <sys/time.h>
namespace myTime{
    using namespace std;
    void timeCalc(timeval a , timeval b){
        if (((b.tv_sec - a.tv_sec)*1000000 + b.tv_usec -a.tv_usec)>=0) {
            if (b.tv_usec > a.tv_usec) {
                cout<<b.tv_sec-a.tv_sec<<"s"<<0.001*(b.tv_usec-a.tv_usec)<<"ms";
            }else{
                cout<<b.tv_sec-a.tv_sec-1<<"s"<<0.001*(b.tv_usec-a.tv_usec)+1000<<"ms";
            }
        }else{
            timeval temp = b;
            b=a;
            a=temp;
            timeCalc(a, b);
        }
    }
};