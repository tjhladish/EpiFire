#include "backgroundthread.h"

BackgroundThread::BackgroundThread(MainWindow* w) { 
      mw=w; 
      _stopped=true;
      setTerminationEnabled(true);

       
     connect(this, SIGNAL(statusChanged(QString)), 
                    mw->statusBar(), SLOT(showMessage(QString)));

     connect(this, SIGNAL(statusChanged(QString)), 
                    mw->progressDialog, SLOT(setLabelText(QString)));

     connect(this, SIGNAL(setProgressValue(int)), 
                    mw->progressDialog, SLOT(setValue(int)));


     connect(this, SIGNAL(started()), mw->progressDialog, SLOT(show())); 
     connect(this, SIGNAL(finished()), mw->progressDialog, SLOT(hide())); 
     //connect(mw->progressDialog,SIGNAL(canceled()),this,SLOT(stop()));
     ///connect(mw->progressDialog,SIGNAL(canceled()),this,SLOT(terminate()));
}

void BackgroundThread::stop() {
    _stopped=true;
    emit statusChanged("Stopping...please wait");
//    mw->network->stopped(true); //flag set 
}



void BackgroundThread::run(void) { 
    _stopped=false;

    emit statusChanged("Starting.. ");

    if (type == GENERATENET ) {
        emit setProgressValue(0);
        emit statusChanged("Generating Network");
        while(1) { mw->network->size(); usleep(1); }
///        bool success = mw->generate_network();
        emit setProgressValue(100);
        //emit completed(success);
        //statusBar()->showMessage(simulateMsg);
    } else if (type == SIMULATENET ) {
        int p=0;
        while(1) {
            emit setProgressValue(p++ / 100000);
            QCoreApplication::processEvents();
            if (_stopped) break;
        }

    }

    if (_stopped ) {
        emit statusChanged("Terminated.. ");
    } else {
        emit statusChanged("Done.. ");
    }

}

