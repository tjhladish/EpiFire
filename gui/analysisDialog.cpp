#include "analysisDialog.h"

AnalysisDialog::AnalysisDialog(MainWindow* w, DialogType d, QString title) {
    mw = w;
    dialogType = d;
    network = mw->network;
    this->setWindowTitle(title);
    thresholdEdited = false;
    
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


void AnalysisDialog::_addResultsAnalysisRow(QGridLayout* layout, QString text, QLineEdit* all, QLineEdit* out, QLineEdit* epi){
    all->setAlignment(Qt::AlignRight);
    out->setAlignment(Qt::AlignRight);
    epi->setAlignment(Qt::AlignRight);

    makeReadonly(all);
    makeReadonly(out);
    makeReadonly(epi);

    int r = layout->rowCount();
    QLabel* lab = new QLabel(text, this);
    layout->addWidget(lab, r, 0);
    layout->addWidget(all, r, 1);
    layout->addWidget(out, r, 2);
    layout->addWidget(epi, r, 3);
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

    degDistPlot = new PlotView(this, "Degree distribution", "Degree", "Frequency");
    degDistPlot->setPlotType(PlotView::DEGPLOT);
    
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
    QGridLayout* resultsBottomLayout = new QGridLayout();

    thresholdEdit = new QLineEdit(this);
    thresholdEdit ->setText(QString::number( 0 ));
    thresholdEdit->setValidator( new QDoubleValidator(thresholdEdit) );
    connect(thresholdEdit, SIGNAL(textChanged(QString)), this, SLOT(updateResultsAnalysis()));

    QPushButton* defaultThresholdButton = new QPushButton("Default");
    connect(defaultThresholdButton, SIGNAL(clicked()), this, SLOT(reset_epi_threshold()));

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
     
    QLabel* thresholdLabel = new QLabel("Outbreak/epidemic threshold:", this);
    resultsTopLayout->addWidget(thresholdLabel, 1, 0, 1, 2); 
    resultsTopLayout->addWidget(thresholdEdit, 1, 2); 
    resultsTopLayout->addWidget(defaultThresholdButton, 1, 3); 

    QLabel* allLabel = new QLabel("All simulations", this);
    QLabel* outLabel = new QLabel("Outbreaks only", this);
    QLabel* epiLabel = new QLabel("Epidemics only", this);

    resultsTopLayout->addWidget(allLabel,   2, 1);
    resultsTopLayout->addWidget(outLabel,   2, 2);
    resultsTopLayout->addWidget(epiLabel,   2, 3);

    _addResultsAnalysisRow(resultsTopLayout, "N",     allNEdit,    outNEdit,    epiNEdit);
    _addResultsAnalysisRow(resultsTopLayout, "Min" ,  allMinEdit,  outMinEdit,  epiMinEdit);
    _addResultsAnalysisRow(resultsTopLayout, "Max" ,  allMaxEdit,  outMaxEdit,  epiMaxEdit);
    _addResultsAnalysisRow(resultsTopLayout, "Mean" , allMeanEdit, outMeanEdit, epiMeanEdit);
    _addResultsAnalysisRow(resultsTopLayout, "SD" ,   allSDEdit,   outSDEdit,   epiSDEdit);

    QGroupBox* resultsAnalysisTop = new QGroupBox();
    resultsAnalysisTop->setLayout(resultsTopLayout);

    QLabel* nbins = new QLabel("Number of bins", this);
    QLabel* minRange = new QLabel("Range minimum", this);
    QLabel* maxRange = new QLabel("Range maximum", this);

    resultsBottomLayout->addWidget(nbins,    1, 0);
    resultsBottomLayout->addWidget(minRange, 1, 1);
    resultsBottomLayout->addWidget(maxRange, 1, 2);

    QLineEdit* nbinsLineEdit = new QLineEdit(this);
    QLineEdit* minRangeLineEdit = new QLineEdit(this);
    QLineEdit* maxRangeLineEdit = new QLineEdit(this);

    resultsBottomLayout->addWidget(nbinsLineEdit,    2, 0);
    resultsBottomLayout->addWidget(minRangeLineEdit, 2, 1);
    resultsBottomLayout->addWidget(maxRangeLineEdit, 2, 2);

    QGroupBox* resultsAnalysisBottom = new QGroupBox();
    resultsAnalysisBottom->setLayout(resultsBottomLayout);
    
    resultsHistPlot = new PlotView(this, "Epidemic size distribution", "Epidemic size", "Frequency");
    resultsHistPlot->setPlotType(PlotView::RESULTS_HISTPLOT);
    
    connect(thresholdEdit, SIGNAL(textChanged(QString)), resultsHistPlot, SLOT(setCutoff(QString)));
    connect(thresholdEdit, SIGNAL(textEdited(QString)), this, SLOT(setThresholdEdited()));
    connect(nbinsLineEdit, SIGNAL(textChanged(QString)), resultsHistPlot, SLOT(setNBins(QString)));
    connect(minRangeLineEdit, SIGNAL(textChanged(QString)), resultsHistPlot, SLOT(setRangeMin(QString)));
    connect(maxRangeLineEdit, SIGNAL(textChanged(QString)), resultsHistPlot, SLOT(setRangeMax(QString)));
    
    // add a close window button
    QPushButton* closeButton = new QPushButton("Close analysis", this);
    connect(closeButton,  SIGNAL(clicked()), this, SLOT(close()));
    QHBoxLayout* buttonBoxLayout   = new QHBoxLayout();
    QWidget* buttonBox = new QWidget();
    buttonBoxLayout->addStretch(1);
    buttonBoxLayout->addWidget(closeButton);
    buttonBox->setLayout(buttonBoxLayout);

    resultsAnalysisLayout->addWidget(resultsAnalysisTop);
    resultsAnalysisLayout->addWidget(resultsHistPlot);
    resultsAnalysisLayout->addWidget(resultsAnalysisBottom);
    resultsAnalysisLayout->addWidget(buttonBox);
    
    this->setLayout(resultsAnalysisLayout);
}

void AnalysisDialog::analyzeNetwork() {
    network = mw->network;
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
    updateResultsAnalysis();
    this->show();
}


void _calcStats( vector<int> &data, vector<QString> &stats ) {
    if (data.empty()) {
        stats[0] = "Undefined";
        stats[1] = "Undefined";
        stats[2] = "Undefined";
        stats[3] = "Undefined";
    } else {
        stats[0] = QString::number( mean(data)        );
        stats[1] = QString::number( stdev(data)       );
        stats[2] = QString::number( min_element(data) );
        stats[3] = QString::number( max_element(data) );
    }
    if (data.size() == 1) {
        stats[1] = "Undefined";
    }
}


void AnalysisDialog::updateResultsAnalysis() {
    vector< vector<int> > histData = mw->histPlot->getData();
    if (histData.size() > 0 and histData[0].size() > 0) {
        vector<int> all_data = mw->histPlot->getData()[0];

        resultsHistPlot->clearData();
        resultsHistPlot->addData(all_data);
        resultsHistPlot->replot();

        double threshold = thresholdEdited ? thresholdEdit->text().toDouble()
                                           : (double) find_epi_threshold();
        
        thresholdEdit->setText( QString::number( threshold ) );
        
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
    } else {
        outMeanEdit ->setText( "Undefined" );
        outSDEdit   ->setText( "Undefined" );
        outMinEdit  ->setText( "Undefined" );
        outMaxEdit  ->setText( "Undefined" );
        outNEdit    ->setText( "0" );
        epiMeanEdit ->setText( "Undefined" );
        epiSDEdit   ->setText( "Undefined" );
        epiMinEdit  ->setText( "Undefined" );
        epiMaxEdit  ->setText( "Undefined" );
        epiNEdit    ->setText( "0" );
        allMeanEdit ->setText( "Undefined" );
        allSDEdit   ->setText( "Undefined" );
        allMinEdit  ->setText( "Undefined" );
        allMaxEdit  ->setText( "Undefined" );
        allNEdit    ->setText( "0" );
        resultsHistPlot->clearData();
        resultsHistPlot->replot();
    }
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
    vector< vector<double> > pathLengths;
    network->calculate_distances(giant_comp, pathLengths);
    double diam = 0.0;
    double mean = 0.0;
    for (unsigned int i = 0; i<pathLengths.size(); i++) {
        double node_mean = 0.0;
        for (unsigned int j = 0; j<pathLengths[i].size(); j++) {
            //if (i==j) continue;
            diam = pathLengths[i][j] > diam ? pathLengths[i][j] : diam;
            node_mean += pathLengths[i][j];
        }
        node_mean /= pathLengths[i].size();
        mean += node_mean;
    }
    mean /= pathLengths.size();
    diameterEdit->setText(QString::number( diam ));
    meanDistanceEdit->setText(QString::number( mean ));
}

int AnalysisDialog::find_epi_threshold() {
    // quick and dirty way to guess the threshold between outbreaks and epidemics
    vector< vector<int> > plotData = resultsHistPlot->getData();
    if (plotData.size() == 0 or plotData[0].size() == 0) return -1;
    vector<int>& data = plotData[0];
    int min = min_element(data);
    int start = 0; // beginning of trough between outbreaks and epis
    int stop  = 0; // end of trough
    int best_range = 0;
    int second_range = 0;

    int curr_start = -1;
    int curr_stop  = -1;

    vector<int> freqs = tabulate_vector(data);

    for (unsigned int s = min; s < freqs.size(); s++) {
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
    if (best_range > 1.5 * second_range) {
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

