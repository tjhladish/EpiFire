#/usr/bin/qmake /Users/tjhladish/EpiFire/gui/epifire.pro -spec macx-g++ -macx -r CONFIG+=release -o Makefile
#make -j2
#otool -L QtGui.framework/QtGui
#otool -L /Library/Frameworks/QtGui.framework/QtGui
#mkdir -p epifire.app/Contents/Frameworks
#cp /Library/Frameworks/QtCore.framework/Versions/4/QtCore ./epifire.app/Contents/Frameworks/
#cp /Library/Frameworks/QtGui.framework/Versions/4/QtGui ./epifire.app/Contents/Frameworks/
/usr/bin/qmake /Users/tjhladish/EpiFire/gui/epifire.pro -spec macx-g++ -macx -r CONFIG+=release -o Makefile
make -j2
install_name_tool -id @executable_path/../Frameworks/QtCore epifire.app/Contents/Frameworks/QtCore
install_name_tool -id @executable_path/../Frameworks/QtGui epifire.app/Contents/Frameworks/QtGui
install_name_tool -change /Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore epifire.app/Contents/MacOS/epifire 
install_name_tool -change /Library/Frameworks/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui epifire.app/Contents/MacOS/epifire 
install_name_tool -change /Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore epifire.app/Contents/Frameworks/QtGui


install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore epifire.app/Contents/MacOS/epifire 
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui epifire.app/Contents/MacOS/epifire 
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore epifire.app/Contents/Frameworks/QtGui
