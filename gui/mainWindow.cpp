#include "mainWindow.h"

/*#############################################################################
#
#   Layout methods
#
#############################################################################*/


MainWindow::MainWindow() {
// Constructor for the main interface

    centralWidget = new QWidget(this);
    leftBox       = new QGroupBox(this);
    // Allow the leftBox to expand vertically, but not horizontally
    leftBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding));
    rightBox      = new QSplitter(Qt::Vertical, this);

    network = new Network("mynetwork",false);
    simulator = NULL;
    graphWidget = new GraphWidget();
    rep_ct = 0;

    logEditor = new QTextEdit();
    logEditor->setReadOnly(true);
    logEditor->setPlainText(tr("No output yet"));

    epiCurvePlot = new PlotArea(this, "Epidemic curves");
    epiCurvePlot->setPlotType(PlotArea::EPICURVE);

    statePlot = new PlotArea(this, "Node state evolution");
    statePlot->setPlotType(PlotArea::STATEPLOT);

    histPlot = new PlotArea(this, "Histogram of epidemic sizes");
    histPlot->setPlotType(PlotArea::HISTPLOT);
    
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QVBoxLayout *rightLayout = new QVBoxLayout;

    createControlButtonsBox();
    createSettingsBox();
    leftLayout->addWidget(settingsGroupBox, Qt::AlignCenter);
    leftLayout->addWidget(logEditor);
    leftLayout->addWidget(controlButtonsGroupBox);
    leftBox->setLayout(leftLayout);
    leftBox->setFlat(true);

    rightLayout->addWidget(statePlot);
    rightLayout->addWidget(epiCurvePlot);
    rightLayout->addWidget(histPlot);
    rightBox->setLayout(rightLayout);     
    setWindowTitle(tr("EpiFire"));

    createMenu();
    mainLayout->setMenuBar(menuBar);
    mainLayout->addWidget(leftBox);
    mainLayout->addWidget(rightBox);
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    statusBar()->showMessage(generateNetMsg);

    simProgress = new QProgressDialog("Simulation running . . .", "Cancel", 0, 100, this);
    simProgress->setWindowModality(Qt::WindowModal);
    //probValidator = new QDoubleValidator(0.0, 1.0, 20, this);
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
    QAction* showGraphWidget = plotMenu->addAction("Show graph Widget");


    connect(showGraphWidget, SIGNAL(triggered()), this, SLOT(showGraphWidget()));
    //connect(showStatePlot, SIGNAL(triggered()), dockWidget1, SLOT(show()));
    //connect(showEpiPlot, SIGNAL(triggered()), dockWidget2, SLOT(show()));
    
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


    // Put everything together
    networkSettingsGroupBox = new QGroupBox(tr("Step 1: Choose a network"));
    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(1);

    //FIRST COLUMN -- Network stuff
    layout->addWidget(netsourceLabel, 0, 0);
    layout->addWidget(netsourceBox, 0, 1);
    
    //fields for imported net
    layout->addWidget(netfileLabel, 2, 0);
    layout->addWidget(netfileLine, 2, 1);

    //fields for generated net
    layout->addWidget(numnodesLabel, 1, 0);
    layout->addWidget(numnodesLine, 1, 1);
    layout->addWidget(distLabel, 2, 0);
    layout->addWidget(distBox, 2, 1);
    layout->addWidget(param1Label, 3, 0);
    layout->addWidget(param1Line, 3, 1);
    layout->addWidget(param2Label, 4, 0);
    layout->addWidget(param2Line, 4, 1);
    
    networkSettingsGroupBox->setLayout(layout);
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
    layout->addWidget(retainDataCheckBox,6,1,2,2);

    simulatorSettingsGroupBox->setLayout(layout);
}


void MainWindow::createControlButtonsBox() {
//Creates the horizontal control box at the bottom of the interface

    controlButtonsGroupBox = new QGroupBox(tr("Step 3: Profit!"));
    QGridLayout *layout    = new QGridLayout;
    
    clearNetButton = new QPushButton("Clear Network");
    connect(clearNetButton, SIGNAL(clicked()), this, SLOT(clear_network()));
    layout->addWidget(clearNetButton, 0, 0);
    clearNetButton->setEnabled(false);
    
    defaultSettingsButton = new QPushButton("Default Settings");
    connect(defaultSettingsButton, SIGNAL(clicked()), this, SLOT(defaultSettings()));
    layout->addWidget(defaultSettingsButton, 0, 1);

    loadNetButton     = new QPushButton("Import Edge List");
    connect(loadNetButton,     SIGNAL(clicked()), this, SLOT(readEdgeList()));
    layout->addWidget(loadNetButton, 0, 2);

    generateNetButton = new QPushButton("Generate Network");
    connect(generateNetButton, SIGNAL(clicked()), this, SLOT(generate_network()));
    layout->addWidget(generateNetButton, 0, 2);

    clearDataButton = new QPushButton("Clear data");
    connect(clearDataButton, SIGNAL(clicked()), this, SLOT(clear_data()));
    layout->addWidget(clearDataButton, 1, 0);
    clearDataButton->setEnabled(false);

    helpButton = new QPushButton("Help");
    //connect(helpButton, SIGNAL(clicked()), this, SLOT(open_help()));
    layout->addWidget(helpButton, 1, 1);

    runSimulationButton = new QPushButton("Run &Simulation");
    connect(runSimulationButton, SIGNAL(clicked()), this, SLOT(simulatorWrapper()));
    runSimulationButton->setDefault(true);
    layout->addWidget(runSimulationButton, 1, 2);
    runSimulationButton->setEnabled(false);

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
        loadNetButton->show();
        generateNetButton->hide();
        numnodesLine->setText("0");
        distBox->hide();
        distLabel->hide();
        changeParameterLabels(3);
    }                            // generate random net
    else {
        netfileLabel->hide();
        netfileLine->hide();
        loadNetButton->hide();
        generateNetButton->show();

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
    
    } else { // Percolation
        double TCB = (transLine->text()).toDouble();
        int d = (infectiousPeriodLine->text()).toInt();
        transLine->setText( QString::number( convertTCBtoT(TCB, d) ) );
        infectiousPeriodLabel->hide();
        infectiousPeriodLine->hide();
    }
}


void MainWindow::clear_data() {
    epiCurvePlot->clearData();
    epiCurvePlot->replot();

    statePlot->clearData();
    statePlot->replot();
    
    histPlot->clearData();
    histPlot->replot();

    rep_ct = 0;
    appendOutputLine("Epidemic data deleted");
    clearDataButton->setEnabled(false);
    statusBar()->showMessage(clearedDataMsg, 1000);
}


void MainWindow::clear_network() {
    if(network) network->clear_nodes();
    updateRZero();
    appendOutputLine("Network deleted");
    runSimulationButton->setEnabled(false);
    clearNetButton->setEnabled(false);
    statusBar()->showMessage(clearedNetMsg, 1000);
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
    runSimulation(j_max, p_zero, RunID);

    //MAKE PLOTS
    epiCurvePlot->replot();
    statePlot->replot();
    histPlot->replot();

}


void MainWindow::addStateData() {
    vector<int> node_states(100);
    for (int i = 0; i < network->size() && i < node_states.size(); i++) {
        node_states[i] = (int) network->get_node(i)->get_state();
    }

    statePlot->addData(node_states);
}


void MainWindow::runSimulation(int j_max, int patient_zero_ct, string RunID) {
    if(simulator == NULL || network == NULL ) {
        cerr << "ERROR: runSimulation() called with undefined sim and net parameters";
        return;
    }

    double R0 = (rzeroLine->text()).toDouble();
    int n = network->size();
    double I0 = (double) patient_zero_ct / n;
    double predictedSize = (double) patient_zero_ct +  ((double) n - patient_zero_ct) * guessEpiSize(R0, 0, 0.5);
    int currentSize = patient_zero_ct;
    cerr << "Predicted (itr, rec, bin)" << predictedSize << " " << I0 << endl;
    setCursor(Qt::WaitCursor); 
    bool abort = false;

    vector<int> epi_sizes (j_max);
    QList<QString> progress; progress << " --" << " \\" << " |" << " /";
    //time_t start;
    for ( int j = 0; j < j_max; j++) {
        QString rep_str = QString::number(++rep_ct, 10);
        statusBar()->showMessage(busySimMsg % progress[rep_ct % 4]);
        simulator->rand_infect(patient_zero_ct);

        vector<int> epi_curve;
        epi_curve.push_back(simulator->count_infected());

        if (j == j_max - 1) {
            statePlot->clearData();
            statePlot->replot(); // draws a white background when data is cleared
            addStateData();
        }
        //start = time(NULL);     

        while (simulator->count_infected() > 0 ) {
            simProgress->setValue(percent_complete(currentSize, predictedSize));
            if ( ! simProgress->wasCanceled()) {
                //cerr << percent_complete(currentSize, predictedSize) << " " << difftime(time(NULL), start) << endl;
                simulator->step_simulation();
                epi_curve.push_back(simulator->count_infected());
                currentSize = simulator->epidemic_size();
                if (j == j_max - 1) addStateData();
            } else { abort = true; }
        }
        simProgress->setValue(100);
        //cerr << "100\n";
        int epi_size = simulator->epidemic_size();

        QString status_line="Rep: " % rep_str % ", Total infected: " % QString::number(epi_size,10);
        appendOutputLine(status_line);
        cout << "Rep: " << rep_ct << "    Total: " << epi_size << "\n\n";

        epiCurvePlot->addData(epi_curve);

        epi_sizes[j] = epi_size;

        simulator->reset();
    }
    histPlot->addData(epi_sizes);
    setCursor(Qt::ArrowCursor);
    clearDataButton->setEnabled(true);
    statusBar()->showMessage(simDoneMsg, 1000);
    return;
}

void MainWindow::showGraphWidget() { 
    graphWidget->clear();
/*
    network->clear_nodes();
    int n = 5;
    network->populate(n);
    for (int i=0; i<n-1; i++) {
        Node* n1 = network->get_nodes()[i];
        for (int j=i; j<n; j++) {
            Node* n2 = network->get_nodes()[j];
            n1->connect_to(n2);
        }
    }*/


    vector<Edge*> edges = network->get_edges();
    map<Edge*, bool> seen;
    for( int i=0; i < edges.size(); i++ ) {
        if (seen.count(edges[i]->get_complement())) continue;
        seen[edges[i]] = true;
        int id1 = edges[i]->get_start()->get_id();
        int id2 = edges[i]->get_end()->get_id();
        string name1 = QString::number(id1).toStdString();
        string name2 = QString::number(id2).toStdString();
        GNode* n1 = graphWidget->addGNode(name1,0);
        GNode* n2 = graphWidget->addGNode(name2,0);
        GEdge* e = graphWidget->addGEdge(n1,n2,"edgeTag",0);
    }
    graphWidget->setLayoutAlgorithm(GraphWidget::Circular);
    graphWidget->newLayout();
    graphWidget->show();
}


void MainWindow::generate_network() {
    statusBar()->showMessage(busyNetMsg);
    setCursor(Qt::WaitCursor);
    appendOutputLine("Generating network . . . ");

    int n = (numnodesLine->text()).toInt();
    DistType dist_type = (DistType) distBox->currentIndex();
    double param1 = (param1Line->text()).toDouble();
    double param2 = (param2Line->text()).toDouble();

    if(network) delete(network);

    network = new Network("mynetwork", false);
    network->populate(n);
                                 // connect network using the parameters above
    if ( connect_network(network, dist_type, param1, param2) ) {
        updateRZero();
        appendOutput("Done.");
        setCursor(Qt::ArrowCursor);
        runSimulationButton->setEnabled(true);
        clearNetButton->setEnabled(true);
        statusBar()->showMessage(simulateMsg);
    } else {
        appendOutput("Unsuccessful.\nIt may be difficult (or impossible) to generate a network using the specified parameters."); 
        setCursor(Qt::ArrowCursor);
        statusBar()->showMessage(generateNetMsg);
    }
}


bool MainWindow::connect_network (Network* net, DistType dist, double param1, double param2) {
    if (dist == POI) {
        return net->rand_connect_poisson(param1);
    }
    else if (dist == EXP) {
        return net->rand_connect_exponential(param1);
    }
    else if (dist == POW) {
        return net->rand_connect_powerlaw(param1, param2);
    }
    else if (dist == URB) {
        vector<double> dist;
        double deg_array[] = {0, 0, 1, 12, 45, 50, 73, 106, 93, 74, 68, 78, 91, 102, 127, 137, 170, 165, 181, 181, 150, 166, 154, 101, 67, 69, 58, 44, 26, 24, 17, 6, 11, 4, 0, 6, 5, 3, 1, 1, 3, 1, 1, 0, 1, 0, 2};
        dist.assign(deg_array,deg_array+47);
        dist = normalize_dist(dist, sum(dist));
        return net->rand_connect_user(dist);
    }
    else if (dist == CON) {
        vector<double> dist(param1+1, 0);
        dist[param1] = 1;
        return net->rand_connect_user(dist);
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

/*
double MainWindow::guessEpiSize(double R0, double P0) {
    //This calculation is based on the expected epidemic size
    //for a mass action model. See Tildesley & Keeling (JTB, 2009).
    double S0 = 1.0 - P0;
    for (double p = 0.01; p <= 1.0; p += 0.01) {
        cerr << "i: " << p << endl;
        if (S0*(1-exp(-R0 * p)) <= p) return p;
    }
    return 1.0;
}*/


double MainWindow::guessEpiSize(double R0, double P0, double guess) {
    //This calculation is based on the expected epidemic size
    //for a mass action model. See Tildesley & Keeling (JTB, 2009).
    cerr << "r: " << guess << endl;
    double S0 = 1.0 - P0;
    double p = S0*(1-exp(-R0 * guess));
    if (fabs(p-guess) < 0.0001) {return p;}
    else return guessEpiSize(R0, P0, p);
}

/*
double MainWindow::guessEpiSizeB(double R0, double P0) {
    //This calculation is based on the expected epidemic size
    //for a mass action model. See Tildesley & Keeling (JTB, 2009).
    cerr << "b" << endl;
    double guess = 0.5;
    double S0 = 1.0 - P0;
    for (int i = 0; i < 10; i++) {
        cerr << "b: " << guess << endl;
        double p = S0*(1-exp(-R0 * guess));
        if (guess < p) {
            guess += 1.0/pow(2, i+2);
        } else if (guess > p) {
            guess -= 1.0/pow(2, i+2);
        }
    }
    return guess;
}
*/

double MainWindow::convertR0toT(double R0) { return R0 * calculate_T_crit(); }


double MainWindow::convertTtoR0(double T) { return T / calculate_T_crit(); }


double MainWindow::convertTtoTCB (double T, int d) { return 1.0 - pow(1.0 - T, 1.0/(double) d); }


double MainWindow::convertTCBtoT (double TCB, int d) { return 1.0 - pow(1.0 - TCB, d); }


int MainWindow::percent_complete(int current, double predicted) { return current > predicted ? 99 : (int) (100 * current/predicted); }
