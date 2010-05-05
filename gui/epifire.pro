#include( ./epifire.pri )
CONFIG += qt thread warn_off debug 

HEADERS     = mainWindow.h \
   guiSim.h \
   histogram.h \
   plotArea.h \
   plotAxes.h

 SOURCES     = mainWindow.cpp \
               guiSim.cpp \
               ../src/*.cpp \
               main.cpp \
               histogram.cpp \
               plotArea.cpp \
               plotAxes.cpp

INCLUDEPATH += /usr/include/qwt-qt4
LIBS += -lqwt-qt4

 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
