#include "analysisDialog.h"

AnalysisDialog::AnalysisDialog(MainWindow* w, DialogType d, QString title) {
    mw = w;
    dialogType = d;
    network = mw->network;
    //backgroundThread = mw->backgroundThread;

    this->setWindowTitle(title);
    
    if (d == NETWORK) {
        createNetworkAnalysis();
    } else if (d == RESULTS) {
        createResultsAnalysis();
    }
}

void AnalysisDialog::_addNetAnalysisRow(QGridLayout* layout, QString text, QLineEdit* box, QPushButton* button) {
    int r = layout->rowCount();
    QLabel* label = new QLabel(text, this);
    layout->addWidget(label, r, 0);
    layout->addWidget(box, r, 1);
    if (button) layout->addWidget(button, r, 2);
}


void AnalysisDialog::_addResultsAnalysisRow(QGridLayout* layout, QString text, QLineEdit* n, QLineEdit* min, QLineEdit* max, QLineEdit* mean, QLineEdit* sd){
    int r = layout->rowCount();
    QLabel* label = new QLabel(text, this);
    layout->addWidget(label, r, 0);
    layout->addWidget(n,     r, 1);
    layout->addWidget(min,   r, 2);
    layout->addWidget(max,   r, 3);
    layout->addWidget(mean,  r, 4);
    layout->addWidget(sd,    r, 5);
}


void AnalysisDialog::createNetworkAnalysis() {
    QVBoxLayout* netAnalysisLayout = new QVBoxLayout();
    QGridLayout* netTopLayout = new QGridLayout();

    nodeCountEdit        = new QLineEdit(this);
    edgeCountEdit        = new QLineEdit(this);
    meanDegreeEdit       = new QLineEdit(this);
    componentCountEdit   = new QLineEdit(this);
    maxComponentSizeEdit = new QLineEdit(this);
    transitivityEdit     = new QLineEdit(this);
    diameterEdit         = new QLineEdit(this);
    meanDistanceEdit     = new QLineEdit(this);

    makeReadonly(nodeCountEdit);
    makeReadonly(edgeCountEdit);
    makeReadonly(meanDegreeEdit);
    makeReadonly(componentCountEdit);
    makeReadonly(maxComponentSizeEdit);
    makeReadonly(transitivityEdit);
    makeReadonly(diameterEdit);
    makeReadonly(meanDistanceEdit);

    componentButton1   = new QPushButton("Calculate", this);
    componentButton2   = new QPushButton("Calculate", this);
    transitivityButton = new QPushButton("Calculate", this);
    diameterButton     = new QPushButton("Calculate", this);
    meanDistanceButton = new QPushButton("Calculate", this);

    connect(componentButton1,   SIGNAL(clicked()), this, SLOT(generate_comp_thread()));
    connect(componentButton2,   SIGNAL(clicked()), this, SLOT(generate_comp_thread()));
    connect(transitivityButton, SIGNAL(clicked()), this, SLOT(generate_trans_thread()));
    connect(diameterButton,     SIGNAL(clicked()), this, SLOT(generate_dist_thread()));
    connect(meanDistanceButton, SIGNAL(clicked()), this, SLOT(generate_dist_thread()));

    _addNetAnalysisRow(netTopLayout, "Node count:",         nodeCountEdit);
    _addNetAnalysisRow(netTopLayout, "Edge count:",         edgeCountEdit);
    _addNetAnalysisRow(netTopLayout, "Mean degree:",        meanDegreeEdit);
    _addNetAnalysisRow(netTopLayout, "Largest component:",  maxComponentSizeEdit, componentButton1);
    _addNetAnalysisRow(netTopLayout, "Component count:",    componentCountEdit, componentButton2 );
    _addNetAnalysisRow(netTopLayout, "Transitivity:",       transitivityEdit, transitivityButton);
    _addNetAnalysisRow(netTopLayout, "Diameter:",           diameterEdit, diameterButton);
    _addNetAnalysisRow(netTopLayout, "Mean shortest path:", meanDistanceEdit, meanDistanceButton);

    QGroupBox* netAnalysisTop = new QGroupBox();
    netAnalysisTop->setLayout(netTopLayout);

    degDistPlot = new PlotArea(this, "Degree distribution");
    degDistPlot->setPlotType(PlotArea::DEGPLOT);
    
    // add a close window button
    QPushButton* closeButton = new QPushButton("Close analysis", this);
    connect(closeButton,  SIGNAL(clicked()), this, SLOT(close()));
    QHBoxLayout* buttonBoxLayout   = new QHBoxLayout();
    QWidget* buttonBox = new QWidget();
    buttonBoxLayout->addStretch(1);
    buttonBoxLayout->addWidget(closeButton);
    buttonBox->setLayout(buttonBoxLayout);

    netAnalysisLayout->addWidget(netAnalysisTop);
    netAnalysisLayout->addWidget(degDistPlot);
    netAnalysisLayout->addWidget(buttonBox);
    
    this->setLayout(netAnalysisLayout);
}


void AnalysisDialog::createResultsAnalysis() {
    QVBoxLayout* resultsAnalysisLayout = new QVBoxLayout();
    QGridLayout* resultsTopLayout = new QGridLayout();

    thresholdEdit = new QLineEdit(this);
    thresholdEdit ->setText(QString::number( 0 ));
    connect(thresholdEdit, SIGNAL(textChanged(QString)), this, SLOT(updateResultsAnalysis()));

    allNEdit      = new QLineEdit(this);
    allMinEdit    = new QLineEdit(this);
    allMaxEdit    = new QLineEdit(this);
    allMeanEdit   = new QLineEdit(this);
    allSDEdit     = new QLineEdit(this);

    outNEdit      = new QLineEdit(this);
    outMinEdit    = new QLineEdit(this);
    outMaxEdit    = new QLineEdit(this);
    outMeanEdit   = new QLineEdit(this);
    outSDEdit     = new QLineEdit(this);

    epiNEdit      = new QLineEdit(this);
    epiMinEdit    = new QLineEdit(this);
    epiMaxEdit    = new QLineEdit(this);
    epiMeanEdit   = new QLineEdit(this);
    epiSDEdit     = new QLineEdit(this);
     
    makeReadonly( allNEdit  );
    makeReadonly( allMinEdit  );
    makeReadonly( allMaxEdit  );
    makeReadonly( allMeanEdit );
    makeReadonly( allSDEdit   );
    
    makeReadonly( outNEdit  );
    makeReadonly( outMinEdit  );
    makeReadonly( outMaxEdit  );
    makeReadonly( outMeanEdit );
    makeReadonly( outSDEdit   );

    makeReadonly( epiNEdit  );
    makeReadonly( epiMinEdit  );
    makeReadonly( epiMaxEdit  );
    makeReadonly( epiMeanEdit );
    makeReadonly( epiSDEdit   );
 
    QLabel* thresholdLabel = new QLabel("Outbreak/epidemic threshold:", this);
    resultsTopLayout->addWidget(thresholdLabel, 1, 0); 
    resultsTopLayout->addWidget(thresholdEdit, 1, 1); 
    
    QLabel* nLabel   = new QLabel("N",   this);
    QLabel* minLabel = new QLabel("Min", this);
    QLabel* maxLabel = new QLabel("Max", this);
    QLabel* meanLabel = new QLabel("Mean", this);
    QLabel* SDLabel  = new QLabel("SD", this);
    resultsTopLayout->addWidget(nLabel,    2, 1);
    resultsTopLayout->addWidget(minLabel,  2, 2);
    resultsTopLayout->addWidget(maxLabel,  2, 3);
    resultsTopLayout->addWidget(meanLabel, 2, 4);
    resultsTopLayout->addWidget(SDLabel,   2, 5);

    _addResultsAnalysisRow(resultsTopLayout, "All simulations:", allNEdit, allMinEdit, allMaxEdit, allMeanEdit, allSDEdit);
    _addResultsAnalysisRow(resultsTopLayout, "Outbreaks only:" , outNEdit, outMinEdit, outMaxEdit, outMeanEdit, outSDEdit);
    _addResultsAnalysisRow(resultsTopLayout, "Epidemics only:" , epiNEdit, epiMinEdit, epiMaxEdit, epiMeanEdit, epiSDEdit);

    QGroupBox* resultsAnalysisTop = new QGroupBox();
    resultsAnalysisTop->setLayout(resultsTopLayout);

    // add a close window button
    QPushButton* closeButton = new QPushButton("Close analysis", this);
    connect(closeButton,  SIGNAL(clicked()), this, SLOT(close()));
    QHBoxLayout* buttonBoxLayout   = new QHBoxLayout();
    QWidget* buttonBox = new QWidget();
    buttonBoxLayout->addStretch(1);
    buttonBoxLayout->addWidget(closeButton);
    buttonBox->setLayout(buttonBoxLayout);

    resultsAnalysisLayout->addWidget(resultsAnalysisTop);
    //resultsAnalysisLayout->addWidget(histPlot);
    resultsAnalysisLayout->addWidget(buttonBox);
    
    this->setLayout(resultsAnalysisLayout);
}

void AnalysisDialog::analyzeNetwork() {
    if (!network or network->size() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Please generate or import a network first.");
        msgBox.exec();
        return;
    }
    nodeCountEdit        ->setText(QString::number( network->size() ));
    int edge_ct = network->get_edges().size();
    edge_ct = network->is_directed() ? edge_ct : edge_ct / 2;
    edgeCountEdit        ->setText(QString::number( edge_ct ));
    meanDegreeEdit       ->setText(QString::number( network->mean_deg() ));
    componentCountEdit   ->clear();
    maxComponentSizeEdit ->clear();
    transitivityEdit     ->clear();
    diameterEdit         ->clear();
    meanDistanceEdit     ->clear();

    degDistPlot->clearData();
    degDistPlot->addData(network->get_deg_series());
    degDistPlot->replot();

    this->exec();
}


void AnalysisDialog::analyzeResults() {
    if (!mw->histPlot or (mw->histPlot->getData()).size() == 0) {
        QMessageBox msgBox; msgBox.setText("Please run some simulations first."); msgBox.exec();
        return;
    }
    int threshold = find_epi_threshold( mw->histPlot->getData()[0] );
    thresholdEdit->setText( QString::number( threshold ) );
    updateResultsAnalysis();
    this->exec();
}


void _calcStats( vector<int> &data, vector<QString> &stats ) {
    if (data.empty()) {
        stats[0] = "Undefined";
        stats[1] = "Undefined";
        stats[2] = "Undefined";
        stats[3] = "Undefined";
        //stats[4] = "0";
    } else {
        stats[0] = QString::number( mean(data)        );
        stats[1] = QString::number( stdev(data)       );
        stats[2] = QString::number( min_element(data) );
        stats[3] = QString::number( max_element(data) );
        //stats[4] = QString::number( data.size() );
    }
    if (data.size() == 1) {
        stats[1] = "Undefined";
    }
}


void AnalysisDialog::updateResultsAnalysis() {
    double threshold = (thresholdEdit->text()).toDouble();
    vector<int> all_data = (mw->histPlot->getData())[0];
    vector<int> outbreaks;
    vector<int> epidemics;
    for( unsigned int i = 0; i < all_data.size(); i++) {
        if (all_data[i] >= threshold) {
            epidemics.push_back(all_data[i]);
        } else {
            outbreaks.push_back(all_data[i]);
        }
    }

    vector<QString> stats(5);
    
    _calcStats( outbreaks, stats);
    outMeanEdit ->setText( stats[0] );
    outSDEdit   ->setText( stats[1] );
    outMinEdit  ->setText( stats[2] );
    outMaxEdit  ->setText( stats[3] );
    //QString txt = stats[4].append("(").append(QString::number(100.0 * outbreaks.size()/all_data.size())).append("%)");
    outNEdit    ->setText( frequencyFormat((double) outbreaks.size(), (double) all_data.size()) );
    
    _calcStats( epidemics, stats);
    epiMeanEdit ->setText( stats[0] );
    epiSDEdit   ->setText( stats[1] );
    epiMinEdit  ->setText( stats[2] );
    epiMaxEdit  ->setText( stats[3] );
    epiNEdit    ->setText( frequencyFormat((double) epidemics.size(), (double) all_data.size()) );
    
    _calcStats( all_data, stats);
    allMeanEdit ->setText( stats[0] );
    allSDEdit   ->setText( stats[1] );
    allMinEdit  ->setText( stats[2] );
    allMaxEdit  ->setText( stats[3] );
    allNEdit    ->setText( frequencyFormat((double) all_data.size(), (double) all_data.size()) );
}


void AnalysisDialog::calculateComponentStats() {
    vector< vector<Node*> >& netComponents = mw->netComponents;
    if ( netComponents.empty() ) netComponents = network->get_components();
    int count = netComponents.size();
    int biggest = 0;

    for (unsigned int i = 0; i < netComponents.size(); i++) {
        if (netComponents[i].size() > (unsigned) biggest) {
            biggest = netComponents[i].size();
        }
    }

    componentCountEdit   ->setText(QString::number( count ));
    maxComponentSizeEdit ->setText(QString::number( biggest ));
    //setCursor(Qt::ArrowCursor);
}


void AnalysisDialog::calculateTransitivity() {
    if (!network) return;
    vector<Node*> empty;
    transitivityEdit->setText(QString::number( network->transitivity(empty) ));
}


void AnalysisDialog::calculateDistances() {
    if (!network) return;
    mw->progressDialog->setLabelText("Finding biggest component ...");
    calculateComponentStats();
    vector< vector<Node*> >& netComponents = mw->netComponents;
    vector<Node*>& giant_comp = netComponents[0];

    // locate the biggest component in the network
    for (unsigned int i = 1; i < netComponents.size(); i++) {
        if (netComponents[i].size() > giant_comp.size()) {
            giant_comp = netComponents[i];
        }
    }
    
    mw->progressDialog->setLabelText("Finding shortest paths in component ...");
    // calculate the shortest path lengths within it
    //for(unsigned int t = 0; t< (*giant_comp).size(); t++) cerr << (*giant_comp)[t]->get_id() << endl;
    vector< vector<int> > pathLengths = network->calculate_unweighted_distances(giant_comp);
    double diam = 0.0;
    double mean = 0.0;
    for (unsigned int i = 0; i<pathLengths.size(); i++) {
        double node_mean = 0.0;
        for (unsigned int j = 0; j<pathLengths[i].size(); j++) {
            if (i==j) continue;
            //cerr << pathLengths[i][j] << "\t";
            diam = pathLengths[i][j] > diam ? pathLengths[i][j] : diam;
            node_mean += pathLengths[i][j];
        }
        //cerr << endl;
        node_mean /= pathLengths[i].size() - 1;
        mean += node_mean;
    }
    
    mean /= pathLengths.size();
    diameterEdit->setText(QString::number( diam ));
    meanDistanceEdit->setText(QString::number( mean ));
}

int AnalysisDialog::find_epi_threshold(vector<int> data) {
    cerr << "data size: " << data.size() << endl;
    // quick and dirty way to guess the threshold between outbreaks and epidemics
    int min = min_element(data);
    int start = 0; // beginning of trough between outbreaks and epis
    int stop  = 0; // end of trough
    int best_range = 0;
    int second_range = 0;

    int curr_start = -1;
    int curr_stop  = -1;

    vector<int> freqs = tabulate_vector(data);

    for (int s = min; s < freqs.size(); s++) {
        if (freqs[s] == 0 and curr_start == -1) {
            curr_start = s;
        } else if (freqs[s] > 0 and curr_start > -1) {
            curr_stop = s;
            if (curr_stop - curr_start > best_range) {
                start = curr_start;
                stop = curr_stop;
                second_range = best_range;
                best_range = curr_stop - curr_start;
                curr_start = -1;
                curr_stop = -1;
            }
        }
    }
    if (best_range > 2 * second_range) {
        return start + (stop-start)/2;
    } else {
        return -1;
    }
}


void AnalysisDialog::generate_comp_thread() {
    mw->setCursor(Qt::WaitCursor);
    mw->backgroundThread->setThreadType(BackgroundThread::COMPONENTS);
    mw->progressDialog->setLabelText("Determining network components");
    mw->backgroundThread->start();
}

void AnalysisDialog::generate_trans_thread() {
    mw->setCursor(Qt::WaitCursor);
    mw->backgroundThread->setThreadType(BackgroundThread::TRANSITIVITY);
    mw->progressDialog->setLabelText("Calculating transitivity clustering coefficient");
    mw->backgroundThread->start();
}


void AnalysisDialog::generate_dist_thread() {
    mw->setCursor(Qt::WaitCursor);
    mw->backgroundThread->setThreadType(BackgroundThread::DISTANCES);
    mw->progressDialog->setLabelText("Beginning shortest path calculation ...");
    mw->backgroundThread->start();
}




