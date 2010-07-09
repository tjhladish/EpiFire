#ifndef BAKGROUNDTHREAD
#define BAKGROUNDTHREAD

#include <QThread>
#include "mainWindow.h"


class MainWindow;

class BackgroundThread: public QThread {
Q_OBJECT

    public:

        BackgroundThread(MainWindow* w);

        enum ThreadType { GENERATE_NET, COMPONENTS, TRANSITIVITY, DISTANCES, SIMULATE };
        void setThreadType(ThreadType t) { type=t; }
        bool stopped() { return _stopped; }

    public slots:
        void stop();

    signals:
        void statusChanged(QString string);
        void updateDialogText(QString string);
        void completed(bool);
        void setProgressValue(int);
        void showProgressDialog();
        void hideProgressDialog();
        void appendOutputLine(QString);

    protected:
        void run(void);

    private:
        volatile bool _stopped;
        ThreadType type;
        MainWindow* mw;
        void runSimulation();
};

#endif
