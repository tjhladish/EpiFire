 HEADERS     = dialog.h \
   gui_sim.h \
   plotarea.h\
   main.h
 SOURCES     = dialog.cpp \
               gui_sim.cpp \
               ../src/*.cpp \
               main.cpp\
               plotarea.cpp

 # install
 target.path = ./
 sources.files = $$SOURCES $$HEADERS *.pro
 sources.path = ./
 INSTALLS += target sources
