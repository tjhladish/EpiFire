#ifndef DEBUGG
#define DEBUGG

#include "mainWindow.h"
class MainWindow;

extern MainWindow* __mainWindow;
void __update_progress(float x);
    

#ifndef PROG
#define PROG(x) __update_progress(x)
#endif 

#endif
