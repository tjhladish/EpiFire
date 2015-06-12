CONFIG += qt thread release c++11

TEMPLATE += app
QT += gui widgets

HEADERS     = mainWindow.h \
             plotView.h \
             plotAxis.h \
             plotPoint.h \
             plotScene.h \ 
             plotRegion.h \
             plotText.h \
             node.h \
             edge.h \
             graphwidget.h \
             debug.h \
             analysisDialog.h \
             ForceLayout.h \
             Quadtree.h \
             backgroundthread.h

 SOURCES     = mainWindow.cpp \
               ../src/*.cpp \
               main.cpp \
               plotView.cpp \
               plotScene.cpp \
               plotAxis.cpp \
               plotPoint.cpp  \
               plotRegion.cpp \
               node.cpp \
               edge.cpp \
               graphwidget.cpp \
               debug.cpp \
               analysisDialog.cpp \
               backgroundthread.cpp \
               ForceLayout.cpp



 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
