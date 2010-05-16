#include "mainWindow.h"

/*#############################################################################
#
#   Layout methods
#
#############################################################################*/


MainWindow::MainWindow() {
// Constructor for the main interface

    centralWidget = new QWidget(this);

    network = new Network("mynetwork",false);
    simulator = NULL;

    logEditor = new QTextEdit();
    logEditor->setReadOnly(true);
    logEditor->setPlainText(tr("No output yet"));

    epiCurvePlot = new PlotArea(this);
    epiCurvePlot->setPlotType(PlotArea::EPICURVE);

    statePlot = new PlotArea(this);
    statePlot->setPlotType(PlotArea::STATEPLOT);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;

    createControlButtonsBox();
    createSettingsBox();
    mainLayout->addWidget(settingsGroupBox, Qt::AlignCenter);
    mainLayout->addWidget(logEditor);
    mainLayout->addWidget(controlButtonsGroupBox);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    centralWidget->setLayout(mainLayout);

    //centralWidget->setMaximumHeight(700);
    centralWidget->setMaximumWidth(500);
    

    setCentralWidget(centralWidget);

    dockWidget1 = new QDockWidget("Node State Plot", this, Qt::Widget);
    dockWidget1->setWidget(statePlot);
    dockWidget1->setFloating(false);
    //dockWidget1->setMinimumSize(600,228);
    dockWidget1->setAllowedAreas(Qt::AllDockWidgetAreas);

    dockWidget2 = new QDockWidget("Epidemic Curve Plot", this, Qt::Widget);
    dockWidget2->setWidget(epiCurvePlot);
    dockWidget2->setFloating(false);
    //dockWidget2->setMinimumSize(600,300);
    dockWidget2->setAllowedAreas(Qt::AllDockWidgetAreas);

    addDockWidget(Qt::RightDockWidgetArea,dockWidget1);
    addDockWidget(Qt::RightDockWidgetArea,dockWidget2);

    dockWidget1->show();
    dockWidget2->show();

    setWindowTitle(tr("EpiFire"));

    createMenu();
    mainLayout->setMenuBar(menuBar);


    probValidator = new QDoubleValidator(0.0, 1.0, 20, this);
}

void MainWindow::createMenu() {
    //Create 'File' menu
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);

    exitAction = fileMenu->addAction(tr("E&xit"));
    openAction = fileMenu->addAction(tr("&Open"));

    QAction* simulateAction = fileMenu->addAction("Simulate");
    simulateAction->setShortcut(Qt::Key_Enter);

    QAction* saveNetwork = fileMenu->addAction("Save network as edgelist");
    QAction* saveDataAction  = fileMenu->addAction("Save epidemic curve data");
    QAction* savePictureAction = fileMenu->addAction("Save epidemic curve plot");

    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(readEdgeList()));
    connect(simulateAction, SIGNAL(triggered()), this, SLOT(simulatorWrapper()));
    connect(saveNetwork, SIGNAL(triggered()), this, SLOT(saveEdgeList()));
    connect(saveDataAction, SIGNAL(triggered()), epiCurvePlot, SLOT(saveData()));
    connect(savePictureAction, SIGNAL(triggered()), epiCurvePlot, SLOT(savePicture()));
 
    //Create 'Plot' menu
    QMenu* plotMenu = new QMenu(tr("&Plot"), this);
    QAction* showEpiPlot = plotMenu->addAction("Show epidemic curve plot");
    QAction* showStatePlot = plotMenu->addAction("Show state plot");

    connect(showStatePlot, SIGNAL(triggered()), dockWidget1, SLOT(show()));
    connect(showEpiPlot, SIGNAL(triggered()), dockWidget2, SLOT(show()));
    
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(plotMenu);
}

void MainWindow::createSettingsBox() {
    createNetworkSettingsBox();
    createSimulatorSettingsBox();
    
    settingsGroupBox = new QGroupBox;
    settingsGroupBox->setFlat(true);
    QVBoxLayout* settingsLayout = new QVBoxLayout;
    settingsLayout->addWidget(networkSettingsGroupBox);    
    settingsLayout->addWidget(simulatorSettingsGroupBox);    

    settingsGroupBox->setLayout(settingsLayout);
    settingsGroupBox->setMaximumHeight(500);

    defaultSettings();
};

void MainWindow::createNetworkSettingsBox() {
// Creates the main input forms and their labels

    // Define text boxes
    numnodesLine = new QLineEdit();
    numnodesLine->setAlignment(Qt::AlignRight);
    param1Line = new QLineEdit();
    param1Line->setAlignment(Qt::AlignRight);
    param2Line = new QLineEdit();
    param2Line->setAlignment(Qt::AlignRight);
    
    netsourceLabel = new QLabel(tr("Network source:"));
    netfileLabel = new QLabel(tr("Filename"));
    netfileLine = new QLineEdit();
    clearnetButton = new QPushButton("Clear Network");
    generatenetButton = new QPushButton("Generate Network");
    loadnetButton     = new QPushButton("Import Edge List");

    netsourceBox= new QComboBox(this);
    netsourceBox->addItem("Generate");
    netsourceBox->addItem("Load from file");

    // Define all of the labels, in order of appearance

    QLabel *numnodesLabel = new QLabel(tr("Number of nodes:"));
    distLabel = new QLabel(tr("Degree distribution:"));
    param1Label = new QLabel(tr("Parameter 1 value:"));
    param2Label = new QLabel(tr("Parameter 2 value:"));

    // Build degree distribution dropdown box
    distBox = new QComboBox;
    distBox->addItem("Poisson");
    distBox->addItem("Exponential");
    distBox->addItem("Power law");
    distBox->addItem("Urban");
    distBox->addItem("Constant");

    // Initialize layout to parameters for first distribution listed, and listen for changes
    changeParameterLabels(0);
    connect(distBox,SIGNAL(currentIndexChanged (int)), this, SLOT(changeParameterLabels(int)));

    changeNetSource(0);
    connect(netsourceBox,SIGNAL(currentIndexChanged (int)), this, SLOT(changeNetSource(int)));

    connect(clearnetButton,    SIGNAL(clicked()), this, SLOT(clear_network()));
    connect(loadnetButton,     SIGNAL(clicked()), this, SLOT(readEdgeList()));
    connect(generatenetButton, SIGNAL(clicked()), this, SLOT(generate_network()));

    // Put everything together
    networkSettingsGroupBox = new QGroupBox(tr("Step 1: Build a network"));
    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(1);

    //FIRST COLUMN -- Network stuff
    layout->addWidget(netsourceLabel, 0, 0);
    layout->addWidget(netsourceBox, 0, 1);
    layout->addWidget(clearnetButton, 5, 0);
    
    //fields for imported net
    layout->addWidget(netfileLabel, 2, 0);
    layout->addWidget(netfileLine, 2, 1);
    layout->addWidget(loadnetButton, 5, 1);

    //fields for generated net
    layout->addWidget(numnodesLabel, 1, 0);
    layout->addWidget(numnodesLine, 1, 1);
    layout->addWidget(distLabel, 2, 0);
    layout->addWidget(distBox, 2, 1);
    layout->addWidget(param1Label, 3, 0);
    layout->addWidget(param1Line, 3, 1);
    layout->addWidget(param2Label, 4, 0);
    layout->addWidget(param2Line, 4, 1);
    layout->addWidget(generatenetButton, 5,1);
    
        networkSettingsGroupBox->setLayout(layout);
    cerr << "net row spacing: " << layout->verticalSpacing() << endl;
}

void MainWindow::createSimulatorSettingsBox() {
    simLabel = new QLabel("Simulation type");
    simBox  =  new QComboBox(this);
    simBox->addItem("Chain Binomial");
    simBox->addItem("Percolation");

    QLabel *numrunsLabel = new QLabel(tr("Number of runs:"));
    numrunsLine = new QLineEdit();
    numrunsLine->setAlignment(Qt::AlignRight);
    rzeroLine = new QLineEdit();
    makeReadonly(rzeroLine);
    rzeroLine->setAlignment(Qt::AlignRight);
    transLine = new QLineEdit();
    transLine->setAlignment(Qt::AlignRight);
//    transLine->setValidator(probValidator);
    pzeroLine = new QLineEdit();
    pzeroLine->setAlignment(Qt::AlignRight);
    infectiousPeriodLine = new QLineEdit();
    infectiousPeriodLine->setAlignment(Qt::AlignRight);

    QLabel *pzeroLabel = new QLabel(tr("Patient zero count:"));
    QLabel *rzeroLabel = new QLabel(tr("Expected R-zero:"));
    QLabel *transLabel = new QLabel(tr("Transmissibility:"));
    infectiousPeriodLabel = new QLabel(tr("Infectious period:"));
    changeSimType(0); 
    connect(simBox,SIGNAL(currentIndexChanged (int)), this, SLOT(changeSimType(int)));
    connect(transLine,            SIGNAL(textChanged(QString)), this, SLOT(updateRZero()));
    connect(infectiousPeriodLine, SIGNAL(textChanged(QString)), this, SLOT(updateRZero()));

    //Build checkbox
    retainDataCheckBox = new QCheckBox(tr("Retain data between runs"));

    // Put everything together
    simulatorSettingsGroupBox = new QGroupBox(tr("Step 2: Design a simulation"));
    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(1);

    //SECOND COLUMN -- Simulation stuff
    layout->addWidget(simLabel, 0, 1);
    layout->addWidget(simBox, 0, 2);
    layout->addWidget(infectiousPeriodLabel, 1, 1);
    layout->addWidget(infectiousPeriodLine, 1, 2);
    layout->addWidget(transLabel, 2, 1);
    layout->addWidget(transLine, 2, 2);
    layout->addWidget(rzeroLabel, 3, 1);
    layout->addWidget(rzeroLine, 3, 2);
    layout->addWidget(pzeroLabel, 4, 1);
    layout->addWidget(pzeroLine, 4, 2);
    layout->addWidget(numrunsLabel, 5, 1);
    layout->addWidget(numrunsLine, 5, 2);
    layout->addWidget(retainDataCheckBox,6,1);

    simulatorSettingsGroupBox->setLayout(layout);
//cerr << "sim row height: "  << layout->rowPreferredHeight(2) << endl;
cerr << "sim row spacing: " << layout->verticalSpacing() << endl;
//cerr << "sim row stretch: " << layout->rowStretchFactor(2) << endl;
}


void MainWindow::createControlButtonsBox() {
//Creates the horizontal control box at the bottom of the interface

    controlButtonsGroupBox = new QGroupBox(tr("Step 3: Profit!"));
    QHBoxLayout *layout = new QHBoxLayout;

    buttons[0] = new QPushButton("Clear data");
    connect(buttons[0], SIGNAL(clicked()), this, SLOT(clear_data()));


    buttons[1] = new QPushButton("Default Settings");
    connect(buttons[1], SIGNAL(clicked()), this, SLOT(defaultSettings()));

    //buttons[2] = new QPushButton("Help");
    //buttons[3] = new QPushButton("Exit");
    //connect(buttons[3], SIGNAL(clicked()), this, SLOT(close()));
    
    buttons[2] = new QPushButton("Run &Simulation");
    connect(buttons[2], SIGNAL(clicked()), this, SLOT(simulatorWrapper()));
    //buttons[0]->setDefault(true);

    for (int i = 0; i < 3; ++i) {
        layout->addWidget(buttons[i]);
    }
    controlButtonsGroupBox->setLayout(layout);
}


/*#############################################################################
#
#   IO methods
#
#############################################################################*/


void MainWindow::saveEdgeList() {

    if(!network || network->size() == 0) { appendOutputLine("No network to save.");   return;}

    QString startdir = ".";
    QString filename = QFileDialog::getSaveFileName(
        this, "Select file to save to:", startdir, "All Files(*.*)");

    if (filename.size() == 0) return;

    network->write_edgelist(filename.toStdString());
}


void MainWindow::readEdgeList() {

    QString startdir = ".";
    QStringList filelist = QFileDialog::getOpenFileNames(
        this, "Select edge list file to load:", startdir, "All Files(*.*)");

    if (filelist.size() == 0) return;
    QString fileName = filelist[0];

    if(network) { delete(network); }

    network = new Network("mynetwork", false);
    network->read_edgelist(fileName.toStdString());
    network->dumper();
    netfileLine->setText(fileName);
    numnodesLine->setText(QString::number(network->size()));
    updateRZero();
}


void MainWindow::appendOutput(QString s) {
// Used to append output to the main textbox
    logEditor->moveCursor( QTextCursor::End) ;
    logEditor->insertPlainText(s);
}


// Used to append new 'paragraph' to the main textbox
void MainWindow::appendOutputLine(QString s) { logEditor->append(s); }


void MainWindow::makeReadonly(QLineEdit* lineEdit) {
    lineEdit->setReadOnly(true);
    QPalette pal = lineEdit->palette();
    pal.setColor(lineEdit->backgroundRole(), Qt::transparent);
    lineEdit->setPalette(pal);
}


/*#############################################################################
#
#   Update methods
#
#############################################################################*/


void MainWindow::defaultSettings() {
//Resets GUI to its default settings (as specified in .h file)
    netsourceBox->setCurrentIndex(0);
    distBox->setCurrentIndex(0);
    changeParameterLabels(0);
    numnodesLine->setText(default_network_size);

    simBox->setCurrentIndex(0);
    changeSimType(0);
    infectiousPeriodLine->setText(default_infectious_pd);
    transLine->setText(default_T);
    numrunsLine->setText(default_num_runs);
    pzeroLine->setText(default_P0);
    retainDataCheckBox->setChecked(true);
}


void MainWindow::changeNetSource(int source) {
    if(source == 1 ) {           // load net from file
        netfileLabel->show();
        netfileLine->show();
        makeReadonly(netfileLine);
        clearnetButton->show();
        loadnetButton->show();
        generatenetButton->hide();
        numnodesLine->setText("0");
        distBox->hide();
        distLabel->hide();
        changeParameterLabels(3);
    }                            // generate random net
    else {
        netfileLabel->hide();
        netfileLine->hide();
        clearnetButton->show();
        loadnetButton->hide();
        generatenetButton->show();

        distBox->show();
        distLabel->show();
        numnodesLine->setText(default_network_size);

        param1Label->show();
        param2Label->show();
        changeParameterLabels(0);
    }
}


void MainWindow::changeParameterLabels(int dist_type) {
//Changes the labels for the parameter boxes, and grays them out as appropriate

    if (dist_type == 0) {
        param1Line->setVisible(1);
        param1Label->setText("Lambda:");
	param1Label->show();
        param1Line->setText("3.0");
        param2Label->hide();
        param2Line->hide();
    }
    else if (dist_type == 1) {
        param1Line->setVisible(1);
        param1Label->setText("Beta:");
        param1Label->show();
	param1Line->setText("0.3");
        param2Label->hide();
        param2Line->hide();
    }
    else if (dist_type == 2) {
        param1Line->show();
	param1Label->show();
        param1Line->setText("1.0");
        param1Label->setText("Alpha:");
        param2Line->show();
        param2Label->show();
        param2Label->setText("Kappa:");
        param2Line->setText("2.0");
    }
    else if (dist_type == 3) {
        param1Line->hide();
        param1Label->hide();
        param2Line->hide();
        param2Label->hide();
    }
    else if (dist_type == 4) {
        param1Line->setVisible(1);
        param1Line->setText("3");
	param1Label->show();
        param1Label->setText("Fixed degree:");
        param2Line->hide();
        param2Label->hide();
    }

}


void MainWindow::changeSimType(int type) {
    epiCurvePlot->clearData();
    epiCurvePlot->replot();
    if (type == 0) { // Chain Binomial
        double T = (transLine->text()).toDouble();
        int d = (infectiousPeriodLine->text()).toInt();
        transLine->setText( QString::number( convertTtoTCB(T, d) ) );
        infectiousPeriodLabel->show();
        infectiousPeriodLine->show();
        //updateRZero();
    
    } else { // Percolation
        double TCB = (transLine->text()).toDouble();
        int d = (infectiousPeriodLine->text()).toInt();
        transLine->setText( QString::number( convertTCBtoT(TCB, d) ) );
        infectiousPeriodLabel->hide();
        infectiousPeriodLine->hide();
        //updateRZero();
    }
}


void MainWindow::clear_data() {
    epiCurvePlot->clearData();
    epiCurvePlot->replot();

    statePlot->clearData();
    statePlot->replot();
    
    appendOutputLine("Epidemic data deleted");
}


void MainWindow::clear_network() {
    if(network) network->clear_nodes();
    updateRZero();
    appendOutputLine("Network deleted");
}


void MainWindow::updateRZero() {
    if (!network || network->size() == 0) {
        rzeroLine->setText( "Undefined" );
        return;
    }

    double T = (transLine->text()).toDouble();
    int d = (infectiousPeriodLine->text()).toInt();
    if ( simBox->currentText() == "Chain Binomial") {
        T = convertTCBtoT(T, d); // convert to perc's transmissibility
    }

    double R0 = convertTtoR0(T); 
    rzeroLine->setText( QString::number(R0));
}

void MainWindow::makeHistogram(int* data_series, int num_runs, int pop_size) {
/*  // to be relocated as a new epiCurvePlot method
    QwtPlot *plot= new QwtPlot(this);

    plot->setCanvasBackground(QColor(Qt::white));
    plot->setTitle("Histogram");

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(plot);

    //HistogramItem *histogram = new HistogramItem();
    //histogram->setColor(Qt::blue);

    const int numValues = 20;    //Number of bins

    //Perform the actual binning

    int histogramValues[numValues]={0};
    int bin;

    for (int i=0; i<num_runs; i++) {
        bin=floor((((float)*data_series)/((float)pop_size))*numValues);
        histogramValues[bin]++;
        data_series++;
    }

    QwtArray<QwtDoubleInterval> intervals(numValues);
    QwtArray<double> values(numValues);

    double pos = 0.0;
    for ( int i = 0; i < (int)intervals.size(); i++ ) {
        const double width = 100.0/(float)numValues;

        intervals[i] = QwtDoubleInterval(pos, pos + double(width));
        values[i] = histogramValues[i];

        pos += width;
    }

    /*histogram->setData(QwtIntervalData(intervals, values));
    histogram->attach(plot);

    plot->setAxisScale(QwtPlot::yLeft, 0.0, num_runs);
    plot->setAxisScale(QwtPlot::xBottom, 0.0, 100.0);
    plot->replot();

    plot->resize(400,300);

    plot->setWindowFlags(Qt::Drawer);
    plot->setEnabled(1);
    plot->show();
    plot->activateWindow();
    plot->raise();
    */
}


/*#############################################################################
#
#   Epidemiology/network methods
#
#############################################################################*/


void MainWindow::simulatorWrapper() {
//Connects the GUI information to the percolation simulator
    if (!network || network->size() == 0 ) { appendOutputLine("Network must be generated first."); return; }

    // Get values from textboxes
    int j_max = (numrunsLine->text()).toInt();
    double T = (transLine->text()).toDouble();
    int p_zero = (pzeroLine->text()).toInt();
    string RunID="1";            // This needs to be updated
    int dist_size_array[j_max];  //Initiate array that will contain distribution sizes
    int* dist_size_point=dist_size_array;

    //CREATE SIMULATOR
    if(simulator) { delete(simulator); simulator=NULL; }

    if ( simBox->currentText() == "Chain Binomial") {
        int infectious_pd = (infectiousPeriodLine->text()).toInt();
        simulator = new ChainBinomial_Sim(network, infectious_pd, T);
    }
    else {
        simulator = new Percolation_Sim(network);
        ((Percolation_Sim*) simulator)->set_transmissibility(T);
    }

    bool retain_data = retainDataCheckBox->isChecked();
    if (! retain_data) {
        epiCurvePlot->clearData();
    }
    statePlot->clearData();

    //RUN SIMULATION
    runSimulation(j_max, p_zero, RunID, dist_size_point);

    //MAKE PLOTS
    MainWindow::appendOutputLine("Done\n");
    makeHistogram(dist_size_point,j_max,network->size());
    MainWindow::epiCurvePlot->replot();

    statePlot->replot();


}


void MainWindow::addStateData() {
    vector<int> node_states(100);
    for (int i = 0; i < network->size() && i < node_states.size(); i++) {
        node_states[i] = (int) network->get_node(i)->get_state();
    }

    statePlot->addData(node_states);
}


void MainWindow::runSimulation(int j_max, int patient_zero_ct, string RunID, int* dist_size_loc) {
    if(simulator == NULL || network == NULL ) {
        cerr << "ERROR: runSimulation() called with undefined sim and net parameters";
        return;
    }

    appendOutputLine("Simulation running . . . ");

    for ( int j = 0; j < j_max; j++) {
        simulator->rand_infect(patient_zero_ct);

        vector<int> epi_curve;
        epi_curve.push_back(simulator->count_infected());

        //vector<int> cache(0,1000);
        if (j == j_max - 1) {
            statePlot->clearData();
            statePlot->replot(); // draws a white background when data is cleared
            addStateData();
        }
             
        while (simulator->count_infected() > 0) {
            simulator->step_simulation();
            epi_curve.push_back(simulator->count_infected());
            if (j == j_max - 1) addStateData();
        }

        int epi_size = simulator->epidemic_size();

        QString status_line="Rep: ";
        status_line.append(QString::number(j+1, 10));
        status_line.append(", Total infected: ");
        status_line.append(QString::number(epi_size,10));
        appendOutputLine(status_line);
        cout << "Rep: " << j << "    Total: " << epi_size << "\n\n";

        epiCurvePlot->addData(epi_curve);

        //Use pointer to report epidemic size back to MainWindow class
        *dist_size_loc = epi_size;
        dist_size_loc++;

        cout << RunID << " " << j << " " << simulator->epidemic_size() << " ";

        //sim.summary();
        simulator->reset();
    }
    return;
}


void MainWindow::generate_network() {
    appendOutputLine("Generating network . . . ");
    int n = (numnodesLine->text()).toInt();
    DistType dist_type = (DistType) distBox->currentIndex();
    double param1 = (param1Line->text()).toDouble();
    double param2 = (param2Line->text()).toDouble();

    if(network) delete(network);

    network = new Network("mynetwork", false);
    network->populate(n);
                                 // connect network using the parameters above
    connect_network(network, dist_type, param1, param2);
    updateRZero();
    appendOutput("Done.");
}


void MainWindow::connect_network (Network* net, DistType dist, double param1, double param2) {
    if (dist == POI) {
        net->rand_connect_poisson(param1);
    }
    else if (dist == EXP) {
        net->rand_connect_exponential(param1);
    }
    else if (dist == POW) {
        net->rand_connect_powerlaw(param1, param2);
    }
    else if (dist == URB) {
        vector<double> dist;
        double deg_array[] = {0, 0, 1, 12, 45, 50, 73, 106, 93, 74, 68, 78, 91, 102, 127, 137, 170, 165, 181, 181, 150, 166, 154, 101, 67, 69, 58, 44, 26, 24, 17, 6, 11, 4, 0, 6, 5, 3, 1, 1, 3, 1, 1, 0, 1, 0, 2};
        dist.assign(deg_array,deg_array+47);
        dist = normalize_dist(dist, sum(dist));
        net->rand_connect_user(dist);
    }
    else if (dist == CON) {
        vector<double> dist(param1+1, 0);
        dist[param1] = 1;
        net->rand_connect_user(dist);
    }
}


double MainWindow::calculate_T_crit() {
    vector<double> dist = network->get_gen_deg_dist();
    double numerator = 0;// mean degree, (= <k>)
    // mean sq(deg) - mean deg (= <k^2> - <k>)
    double denominator = 0;
    for (unsigned int k=1; k < dist.size(); k++) {
        numerator += k * dist[k];
        denominator += k * (k-1) * dist[k];
    }
    return  numerator/denominator;
}

void MainWindow::resizeEvent ( QResizeEvent *event ) {
    return;

    QSize old = event->oldSize();   //-1,-1
    QSize size = event->size();     //770,720
    qDebug() << size << " " << old;

/*
    QSize cSize = centralWidget->size();    //
    //centralWidget->setMaximumSize(700,centralWidget->height());
    dockWidget1->setMaximumSize( size.width()-cSize.width(), dockWidget1->height()-10 );
    dockWidget2->setMaximumSize( size.width()-cSize.width(), dockWidget2->height()-10 );
    //dockWidget1->setMinimumSize( size.width()-cSize.width()-10, dockWidget1->height()-10 );
    //dockWidget2->setMinimumSize( size.width()-cSize.width()-10, dockWidget2->height()-10 );
    dockWidget1->setMinimumSize( 200, dockWidget1->height()-10 );
    dockWidget2->setMinimumSize( 200, dockWidget2->height()-10 );
    */
}

/*
QSize(770, 720)   QSize(-1, -1) 
QSize(771, 721)   QSize(770, 720) 
QSize(774, 725)   QSize(771, 721) 
QSize(774, 726)   QSize(774, 725) 
QSize(774, 727)   QSize(774, 726) 
QSize(774, 731)   QSize(774, 727) 
QSize(781, 752)   QSize(774, 731) 
QSize(782, 761)   QSize(781, 752) 
QSize(791, 782)   QSize(782, 761) 
QSize(793, 783)   QSize(791, 782) 
*/


double MainWindow::convertR0toT(double R0) { return R0 * calculate_T_crit(); }


double MainWindow::convertTtoR0(double T) { return T / calculate_T_crit(); }


double MainWindow::convertTtoTCB (double T, int d) { return 1.0 - pow(1.0 - T, 1.0/(double) d); }


double MainWindow::convertTCBtoT (double TCB, int d) { return 1.0 - pow(1.0 - TCB, d); }



