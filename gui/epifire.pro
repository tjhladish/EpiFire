##include( ./epifire.pri )
CONFIG += qt thread warn_off #debug 

HEADERS     = mainWindow.h \
             plotArea.h \
             plotAxis.h \
             plotPoint.h \
             plotScene.h 

 SOURCES     = mainWindow.cpp \
               ../src/*.cpp \
               main.cpp \
               plotArea.cpp \
               plotAxis.cpp \
               plotPoint.cpp 

INCLUDEPATH += /usr/include/qwt-qt4
#LIBS += -lqwt-qt4

 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
