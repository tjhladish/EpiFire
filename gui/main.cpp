#include <QApplication>
#include "mainWindow.h"

MainWindow* __mainWindow=0;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    __mainWindow = &mainWindow;
    return app.exec();

}
