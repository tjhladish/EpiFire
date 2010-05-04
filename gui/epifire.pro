#include( ./epifire.pri )
CONFIG += qt thread warn_off debug 

HEADERS     = dialog.h \
   gui_sim.h \
   histogram_item.h \
   plotarea.h

 SOURCES     = dialog.cpp \
               gui_sim.cpp \
               ../src/*.cpp \
               main.cpp \
               histogram_item.cpp \
               plotarea.cpp

INCLUDEPATH += /usr/include/qwt-qt4
LIBS += -lqwt-qt4

 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
