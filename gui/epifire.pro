#include( ./epifire.pri )
CONFIG += qt thread warn_off debug 

HEADERS     = mainWindow.h \
              histogram.h \
             plotArea.h \
             plotAxis.h

 SOURCES     = mainWindow.cpp \
               ../src/*.cpp \
               main.cpp \
               histogram.cpp \
               plotArea.cpp \
               plotAxis.cpp

INCLUDEPATH += /usr/include/qwt-qt4
LIBS += -lqwt-qt4

 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
