CONFIG += qt thread release

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
               backgroundthread.cpp



 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
