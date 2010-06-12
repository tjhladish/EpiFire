#include "backgroundthread.h"

BackgroundThread::BackgroundThread(MainWindow* w) { 
      mw=w; 
      _stopped=true;
      setTerminationEnabled(true);

       
     connect(this, SIGNAL(statusChanged(QString)), 
                    mw->statusBar(), SLOT(showMessage(QString)));

     connect(this, SIGNAL(updateDialogText(QString)), 
                    mw->progressDialog, SLOT(setLabelText(QString)));

     connect(this, SIGNAL(setProgressValue(int)), 
                    mw->progressDialog, SLOT(setValue(int)));

     connect(this, SIGNAL(showProgressDialog()), mw->progressDialog, SLOT(show())); 
     connect(this, SIGNAL(hideProgressDialog()), mw->progressDialog, SLOT(hide())); 
}

void BackgroundThread::stop() {
    _stopped=true;
    emit updateDialogText("Stopping . . . please wait");
    if (type != SIMULATE) {
        mw->network->stop_processing();
    }
    while (this->isRunning()) msleep(50);
    mw->network->reset_processing_flag();
    emit hideProgressDialog();
    emit updateDialogText("");
}



void BackgroundThread::run(void) { 
    _stopped=false;

    if (type == GENERATE_NET ) {
        emit setProgressValue(0);
        bool success = mw->generate_network();
        emit setProgressValue(100);
        if (! success) mw->appendOutput("Unsuccessful.\nIt may be difficult (or impossible) to generate a network using these parameters."); 
        emit completed(success);
        //mw->network->reset_progress();
    } else if (type == COMPONENTS ) {
        emit setProgressValue(0);
        mw->calculateComponentStats();
        emit setProgressValue(100);
    } else if (type == TRANSITIVITY ) {
        emit setProgressValue(0);
        mw->calculateTransitivity();
        emit setProgressValue(100);
    } else if (type == DISTANCES ) {
        mw->calculateComponentStats();
        emit setProgressValue(0);
        emit showProgressDialog();
        mw->calculateDistances();
        emit setProgressValue(100);
    } else if (type == SIMULATE ) {
        emit setProgressValue(0);
        runSimulation();
    }
}

void BackgroundThread::runSimulation() {
    if(mw->simulator == NULL || mw->network == NULL ) {
        cerr << "ERROR: runSimulation() called with undefined sim and net parameters";
        return;
    }

    int j_max = (mw->numrunsLine->text()).toInt();
    int patient_zero_ct = (mw->pzeroLine->text()).toInt(); 
    double R0 = (mw->rzeroLine->text()).toDouble();
    int n = mw->network->size();
    
    double I0 = (double) patient_zero_ct / n;
    double predictedSize = (double) patient_zero_ct +  ((double) n - patient_zero_ct) * mw->guessEpiSize(R0, 0, 0.5);
    int currentSize = patient_zero_ct;

    vector<int> epi_sizes (j_max);
    for ( int j = 0; j < j_max; j++) {
        if (_stopped) break;
        QString rep_str = QString::number(++(mw->rep_ct), 10);
        emit statusChanged(busySimMsg);
        mw->simulator->rand_infect(patient_zero_ct);

        vector<int> epi_curve;
        epi_curve.push_back(mw->simulator->count_infected());

        if (j == j_max - 1) {
            mw->statePlot->clearData();
            //mw->statePlot->replot(); // draws a white background when data is cleared
            mw->addStateData();
        }

        while (mw->simulator->count_infected() > 0 && ! _stopped) {
            emit setProgressValue(mw->percent_complete(currentSize, predictedSize));
                mw->simulator->step_simulation();
                epi_curve.push_back(mw->simulator->count_infected());
                currentSize = mw->simulator->epidemic_size();
                if (j == j_max - 1) mw->addStateData();
        }
        emit setProgressValue(100);
        int epi_size = mw->simulator->epidemic_size();

        QString status = "Rep: " + rep_str + ", Total infected: " + QString::number(epi_size,10);
        mw->appendOutputLine(status);
        cerr << "Rep: " << mw->rep_ct << "    Total: " << epi_size << "\n\n";

        if (_stopped) {
            emit statusChanged("Simulation interrupted");
        } else {
            emit statusChanged("Simulation complete");
            mw->epiCurvePlot->addData(epi_curve);
        }
        epi_sizes[j] = epi_size;

        mw->simulator->reset();
    }
    if (! _stopped) mw->histPlot->addData(epi_sizes);
    return;
}


