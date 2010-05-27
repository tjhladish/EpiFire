CONFIG += qt thread warn_off debug

HEADERS     = mainWindow.h \
             plotArea.h \
             plotAxis.h \
             plotPoint.h \
             plotScene.h \ 
             node.h \
             edge.h \
             graphwidget.h

 SOURCES     = mainWindow.cpp \
               ../src/*.cpp \
               main.cpp \
               plotArea.cpp \
               plotAxis.cpp \
               plotPoint.cpp  \
               node.cpp \
               edge.cpp \
               graphwidget.cpp


 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
