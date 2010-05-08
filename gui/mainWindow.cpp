#include "mainWindow.h"

void MainWindow::createMenu() {
//Creates 'File' menu at the top
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    openAction = fileMenu->addAction(tr("&Open"));
    QAction* simulateAction = fileMenu->addAction("Simulate");
    simulateAction->setShortcut(Qt::Key_Enter);

    QAction* saveNetwork = fileMenu->addAction("Save Network To File");
    QAction* saveDataAction  = fileMenu->addAction("Save Simulation Data to File");
    QAction* savePictureAction = fileMenu->addAction("Save Plot to File");

    plotArea = new PlotArea(this);

    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(readEdgeList()));
    connect(simulateAction, SIGNAL(triggered()), this, SLOT(simulate()));
    connect(saveNetwork, SIGNAL(triggered()), this, SLOT(saveEdgeList()));
    connect(saveDataAction, SIGNAL(triggered()), plotArea, SLOT(saveData()));
    connect(savePictureAction, SIGNAL(triggered()), plotArea, SLOT(savePicture()));

    network = new Network("mynetwork",false);
    simulator = NULL;
}


void MainWindow::saveEdgeList() {

    if(!network || network->size() == 0) { appendOutput("No network to save.");   return;}

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


void MainWindow::appendOutput(QString teststring) {
// Used to append output to the main textbox
    bigEditor->append(teststring);
}


void MainWindow::defaultSettings() {
//Resets GUI to its default settings (as specified in .h file)
    distBox->setCurrentIndex(0);
    numrunsLine->setText(default_num_runs);
    numnodesLine->setText(default_network_size);
    transLine->setText(default_T);
    pzeroLine->setText(default_P0);

}


void MainWindow::createHorizontalGroupBox() {
//Creates the horizontal control box at the bottom of the interface

    horizontalGroupBox = new QGroupBox(tr("Control"));
    QHBoxLayout *layout = new QHBoxLayout;

    buttons[0] = new QPushButton("&Simulate");
    connect(buttons[0], SIGNAL(clicked()), this, SLOT(simulate()));
    //buttons[0]->setDefault(true);

    buttons[1] = new QPushButton("Default Settings");
    connect(buttons[1], SIGNAL(clicked()), this, SLOT(defaultSettings()));

    buttons[2] = new QPushButton("Help");
    buttons[3] = new QPushButton("Exit");
    connect(buttons[3], SIGNAL(clicked()), this, SLOT(close()));

    for (int i = 0; i < 4; ++i) {
        layout->addWidget(buttons[i]);
    }
    horizontalGroupBox->setLayout(layout);
}


void MainWindow::changeSimType(int type) {
    if (type == 0) { // Chain Binomial
        infectiousPeriodLabel->show();
        infectiousPeriodLine->show();
        updateRZero();
    
    } else { // Percolation
        infectiousPeriodLabel->hide();
        infectiousPeriodLine->hide();
        updateRZero();
    }
}


void MainWindow::changeNetSource(int source) {
    if(source == 1 ) {           // load net from file
        netfileLabel->show();
        netfileLine->show();
        netfileLine->setReadOnly(true);
        clearnetButton->show();
        loadnetButton->show();
        generatenetButton->hide();
        distBox->hide();
        distLabel->hide();
        changeParameterLabels(3);
    }                            // generate random net
    else {
        netfileLabel->hide();
        netfileLine->hide();
        clearnetButton->hide();
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
        param1Line->setText("3.0");
        param2Label->hide();
        param2Line->hide();
    }
    else if (dist_type == 1) {
        param1Line->setVisible(1);
        param1Label->setText("Beta:");
        param1Line->setText("0.3");
        param2Label->hide();
        param2Line->hide();
    }
    else if (dist_type == 2) {
        param1Line->show();
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
        param1Label->setText("Fixed degree:");
        param2Line->hide();
        param2Label->hide();
    }

}

void MainWindow::updateRZero() {
    cerr << "Update R0\n";
    if (!network || network->size() == 0) {
        rzeroLine->setText( "Undefined" );
        return;
    }

    double T = (transLine->text()).toDouble();
    int d = (infectiousPeriodLine->text()).toInt();
    if ( simBox->currentText() == "Chain Binomial") {
        cerr << "CBT: " << T << endl;
        T = 1.0 - pow(1.0 - T, d);
        cerr << "T: " << T << endl;
    }

    //Calculate critical transmissibility for this network
    vector<double> dist = network->get_gen_deg_dist();
    double numerator = 0;// mean degree, (= <k>)
    // mean sq(deg) - mean deg (= <k^2> - <k>)
    double denominator = 0;
    for (unsigned int k=1; k < dist.size(); k++) {
        numerator += k * dist[k];
        denominator += k * (k-1) * dist[k];
    }
    double T_crit =  numerator/denominator;

    double R0 = T / T_crit; 
    rzeroLine->setText( QString::number(R0));
    cerr << "Done updating\n";
}

void MainWindow::createGridGroupBox() {
// Creates the main input forms and their labels

    // Define text boxes
    numnodesLine = new QLineEdit();
    numnodesLine->setAlignment(Qt::AlignRight);
    param1Line = new QLineEdit();
    param1Line->setAlignment(Qt::AlignRight);
    param2Line = new QLineEdit();
    param2Line->setAlignment(Qt::AlignRight);
    
    numrunsLine = new QLineEdit();
    numrunsLine->setAlignment(Qt::AlignRight);
    rzeroLine = new QLineEdit();
    rzeroLine->setReadOnly(true);
    rzeroLine->setAlignment(Qt::AlignRight);
    transLine = new QLineEdit();
    transLine->setAlignment(Qt::AlignRight);
//    transLine->setValidator(probValidator);
    pzeroLine = new QLineEdit();
    pzeroLine->setAlignment(Qt::AlignRight);
    infectiousPeriodLine = new QLineEdit();
    infectiousPeriodLine->setAlignment(Qt::AlignRight);

    netsourceLabel = new QLabel(tr("Select Network Source"));
    netfileLabel = new QLabel(tr("Filename"));
    netfileLine = new QLineEdit();
    clearnetButton = new QPushButton("Clear Network");
    generatenetButton = new QPushButton("Generate Network");
    loadnetButton     = new QPushButton("Load Network");

    simLabel = new QLabel("Simulation type");

    simBox  =  new QComboBox(this);
    simBox->addItem("Chain Binomial");
    simBox->addItem("Percolation");

    netsourceBox= new QComboBox(this);
    netsourceBox->addItem("Generate");
    netsourceBox->addItem("Load from file");

    // Define all of the labels, in order of appearance

    QLabel *numrunsLabel = new QLabel(tr("Number of runs:"));
    QLabel *numnodesLabel = new QLabel(tr("Number of nodes:"));
    distLabel = new QLabel(tr("Degree distribution:"));
    param1Label = new QLabel(tr("Parameter 1 value:"));
    param2Label = new QLabel(tr("Parameter 2 value:"));
    QLabel *pzeroLabel = new QLabel(tr("Patient zero count:"));
    QLabel *rzeroLabel = new QLabel(tr("Expected R-zero:"));
    QLabel *transLabel = new QLabel(tr("Transmissibility:"));
    infectiousPeriodLabel = new QLabel(tr("Infectious period:"));

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

    changeSimType(0); 
    connect(simBox,SIGNAL(currentIndexChanged (int)), this, SLOT(changeSimType(int)));
    connect(transLine,         SIGNAL(textChanged(QString)), this, SLOT(updateRZero()));

    //Build checkbox
    retainDataCheckBox = new QCheckBox(tr("Retain data between runs"));

    // Put everything together
    gridGroupBox = new QGroupBox(tr("Simulation parameters"));
    QGridLayout *layout = new QGridLayout;

    //FIRST COLUMN -- Network stuff
    layout->addWidget(netsourceLabel, 0, 0);
    layout->addWidget(netsourceBox, 0, 1);
    //fields for imported net
    layout->addWidget(netfileLabel, 2, 0);
    layout->addWidget(netfileLine, 2, 1);
    layout->addWidget(loadnetButton, 3, 0);
    layout->addWidget(clearnetButton, 3, 1);

    //fields for generated net
    layout->addWidget(numnodesLabel, 1, 0);
    layout->addWidget(numnodesLine, 1, 1);
    layout->addWidget(distLabel, 2, 0);
    layout->addWidget(distBox, 2, 1);
    layout->addWidget(param1Label, 3, 0);
    layout->addWidget(param1Line, 3, 1);
    layout->addWidget(param2Label, 4, 0);
    layout->addWidget(param2Line, 4, 1);
    layout->addWidget(generatenetButton, 5,0);

    //SECOND COLUMN -- Simulation stuff
    layout->addWidget(simLabel, 0, 3);
    layout->addWidget(simBox, 0, 4);
    layout->addWidget(infectiousPeriodLabel, 1, 3);
    layout->addWidget(infectiousPeriodLine, 1, 4);
    layout->addWidget(transLabel, 2, 3);
    layout->addWidget(transLine, 2, 4);
    layout->addWidget(rzeroLabel, 3, 3);
    layout->addWidget(rzeroLine, 3, 4);
    layout->addWidget(pzeroLabel, 4, 3);
    layout->addWidget(pzeroLine, 4, 4);
    layout->addWidget(numrunsLabel, 5, 3);
    layout->addWidget(numrunsLine, 5, 4);
    layout->addWidget(retainDataCheckBox,6,3);

    gridGroupBox->setLayout(layout);

    defaultSettings();

}


MainWindow::MainWindow() {
// Constructor for the main interface

    createMenu();
    createHorizontalGroupBox();
    createGridGroupBox();

    QWidget* centralWidget = new QWidget(this);

    bigEditor = new QTextEdit();
    bigEditor->setReadOnly(true);
    bigEditor->setPlainText(tr("No output yet"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMenuBar(menuBar);

    mainLayout->addWidget(gridGroupBox,Qt::AlignCenter);
    mainLayout->addWidget(plotArea);
    mainLayout->addWidget(bigEditor);
    mainLayout->addWidget(horizontalGroupBox);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    setWindowTitle(tr("EpiFire"));
    probValidator = new QDoubleValidator(0.0, 1.0, 20, this);
}


void MainWindow::makeHistogram(int* data_series, int num_runs, int pop_size) {

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


void MainWindow::generate_network() {

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
}


void MainWindow::clear_network() {
    if(network) network->clear_nodes();
    updateRZero();
}


void MainWindow::simulate() {
//Connects the GUI information to the percolation simulator
    if (!network || network->size() == 0 ) { appendOutput("Network must be generated first."); return; }

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
        int infectious_pd = 10;
        simulator = new ChainBinomial_Sim(network, infectious_pd, T);
    }
    else {
        simulator = new Percolation_Sim(network);
        ((Percolation_Sim*) simulator)->set_transmissibility(T);
    }

    //RUN SIMULATION
    vector< vector<int> > epi_curves = simulate_main(j_max, p_zero, RunID, dist_size_point);

    //MAKE PLOTS
    MainWindow::appendOutput("\tDone\n");
    for (unsigned int i =0; i<epi_curves.size(); i++) {
        MainWindow::plotArea->addData(epi_curves[i]);
        QString start_of_line="Epidemic size from run ";
        QString epi_size=QString::number(i+1,10).append(":   ").append(QString::number(dist_size_array[i],10));
        MainWindow::appendOutput(start_of_line.append(epi_size));
    }
    makeHistogram(dist_size_point,j_max,network->size());
    MainWindow::plotArea->replot();

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


vector< vector<int> > MainWindow::simulate_main(int j_max, int patient_zero_ct, string RunID, int* dist_size_loc) {
    // Header line
    //cout << "# RunID Network Season Epi_size P0_size R0\n";

    vector<Node*> patients_zero;
    vector< vector<int> > epi_curves (j_max);

    if(simulator == NULL || network == NULL ) {
        cerr << "ERROR: simulate_main() called with undefined sim and net parameters";
        return epi_curves;
    }

    MainWindow::appendOutput("-----------------------------------");
    MainWindow::appendOutput("Simulation running...");

    for ( int j = 0; j < j_max; j++) {
        simulator->rand_infect(patient_zero_ct);

        vector<int> epi_curve;
        epi_curve.push_back(simulator->count_infected());
        while (simulator->count_infected() > 0) {
            simulator->step_simulation();
            epi_curve.push_back(simulator->count_infected());
        }

        for(unsigned int k=0; k < epi_curve.size(); k++ ) cerr << epi_curve[k]<<endl;

        cout << "Rep: " << j << "    Total: " << simulator->epidemic_size() << "\n\n";
        epi_curves[j] = epi_curve;

        //plotArea->addData(epi_curve);
        //QString simOutput;

        //Use pointer to report epidemic size back to MainWindow class
        *dist_size_loc=simulator->epidemic_size();
        dist_size_loc++;

        cout << RunID << " " << j << " " << simulator->epidemic_size() << " ";

        //sim.summary();
        simulator->reset();
    }
    return epi_curves;
}
