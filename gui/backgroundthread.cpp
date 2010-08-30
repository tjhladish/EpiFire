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
     connect(this, SIGNAL(appendOutputLine(QString)), mw, SLOT(appendOutputLine(QString))); 
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
    //cerr << "Dialog hidden!\n";
    emit updateDialogText("");
}



void BackgroundThread::run(void) { 
    _stopped=false;

    if (type == GENERATE_NET ) {
        emit setProgressValue(0);
        bool success = mw->generate_network();
        emit setProgressValue(100);
        if (! success) emit appendOutputLine("Network generation was unsuccessful.\nIt may be difficult (or impossible) to generate a network using these parameters."); 
        emit completed(success);
    } else if (type == COMPONENTS ) {
        emit setProgressValue(0);
        mw->netAnalysisDialog->calculateComponentStats();
        emit setProgressValue(100);
    } else if (type == TRANSITIVITY ) {
        emit setProgressValue(0);
        mw->netAnalysisDialog->calculateTransitivity();
        emit setProgressValue(100);
    } else if (type == DISTANCES ) {
        mw->netAnalysisDialog->calculateComponentStats();
        emit setProgressValue(0);
        emit showProgressDialog();
        mw->netAnalysisDialog->calculateDistances();
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

    mw->J_max = (mw->numrunsLine->text()).toInt();
    int& j_max = mw->J_max;
    int& j = mw->J;

    int patient_zero_ct = (mw->pzeroLine->text()).toInt(); 
    double predictedSize = (mw->maPredictionLine->text()).toDouble();
    int currentSize = patient_zero_ct;

    vector<int> epi_sizes (j_max);
    
    if (mw->retainDataCheckBox->isChecked() == false) {
        mw->epiCurvePlot->clearData();
        mw->histPlot->clearData();
    }
    
    for ( j = 0; j < j_max; j++) {
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
            mw->updateProgress(mw->percent_complete(currentSize, predictedSize));
            //emit setProgressValue(mw->percent_complete(currentSize, predictedSize));
            mw->simulator->step_simulation();
            epi_curve.push_back(mw->simulator->count_infected());
            currentSize = mw->simulator->epidemic_size();
            if (j == j_max - 1) mw->addStateData();
        }
        if (! _stopped) mw->updateProgress(100);
        //emit setProgressValue(100);
        int epi_size = mw->simulator->epidemic_size();

        QString status = "Rep: " + rep_str + ", Total infected: " + QString::number(epi_size,10);
        emit appendOutputLine(status);
        //cerr << "Rep: " << mw->rep_ct << "    Total: " << epi_size << "\n\n";

        if (_stopped) {
            emit statusChanged("Simulation interrupted");
            epi_sizes.resize(j);
        } else {
            emit statusChanged("Simulation complete");
            mw->epiCurvePlot->addData(epi_curve);
            epi_sizes[j] = epi_size;
        }

        mw->simulator->reset();
    }
    mw->histPlot->addData(epi_sizes);
    //if (! _stopped) mw->histPlot->addData(epi_sizes);
    mw->simulatorBusy = false;
    mw->J = 0;
    mw->J_max = 1;
    return;
}


