#ifndef DEBUGG
#define DEBUGG

#include<iostream>


inline void __update_progress(float) { 
    //cerr << x << endl;
}
    

#ifndef PROG
#define PROG(x) __update_progress(x)
#endif 

#endif
