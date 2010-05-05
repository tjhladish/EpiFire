#include "mainWindow.h"

void MainWindow::createMenu()
//Creates 'File' menu at the top
{
  menuBar = new QMenuBar;

  fileMenu = new QMenu(tr("&File"), this);
  exitAction = fileMenu->addAction(tr("E&xit"));
  openAction = fileMenu->addAction(tr("&Open"));
  QAction* simulateAction = fileMenu->addAction("Simulate");
  simulateAction->setShortcut(Qt::Key_Enter);

  menuBar->addMenu(fileMenu);

  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
  connect(openAction, SIGNAL(triggered()), this, SLOT(readEdgeList()));
  connect(simulateAction, SIGNAL(triggered()), this, SLOT(percolationSim()));

  network = new Network("mynetwork",false);
}

void MainWindow::readEdgeList() {

    QString startdir = ".";
    QStringList filelist = QFileDialog::getOpenFileNames(
           this, "Select edge list file to load:", startdir, "All Files(*.*)");

    if (filelist.size() == 0) return;
    QString fileName = filelist[0];

    network->clear_nodes();
    network->read_edgelist(fileName.toStdString());
}

void MainWindow::appendOutput(QString teststring)
// Used to append output to the main textbox
{
  bigEditor->append(teststring);
}

void MainWindow::defaultSettings()
//Resets GUI to its default settings (as specified in .h file)
{
  distBox->setCurrentIndex(0); 
  numrunsLine->setText(default_num_runs);
  numnodesLine->setText(default_network_size);
  rzeroLine->setText(default_R0);
  pzeroLine->setText(default_P0);
  reuseCheckBox->setChecked(true);
 
}

void MainWindow::createHorizontalGroupBox()
  //Creates the horizontal control box at the bottom of the interface

{
  horizontalGroupBox = new QGroupBox(tr("Control"));
  QHBoxLayout *layout = new QHBoxLayout;

  buttons[0] = new QPushButton("&Simulate");
  connect(buttons[0], SIGNAL(clicked()), this, SLOT(percolationSim()));
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


void MainWindow::changeParameterLabels(int dist_type)
//Changes the labels for the parameter boxes, and grays them out as appropriate
{
  
  if (dist_type == 0) 
    {
      param1Line->setVisible(1);
      param1Label->setText("Lambda:");
      param1Line->setText("3.0");
      param2Line->setVisible(0);
      param2Label->setText("");
      param2Line->setText("");
    }
  else if (dist_type == 1)
    {
     param1Line->setVisible(1);
     param1Label->setText("Beta:");
     param1Line->setText("0.3");
     param2Line->setVisible(0);
     param2Label->setText("");
     param2Line->setText("");
    } 
  else if (dist_type == 2)
    {
      param1Line->setVisible(1);
      param1Line->setText("1.0");
      param1Label->setText("Alpha:");
      param2Line->setVisible(1);
      param2Label->setText("Kappa:");
      param2Line->setText("2.0");
    }
  else if (dist_type == 3)
    {
      param1Line->setVisible(0);
      param1Line->setText("");
      param1Label->setText("");
      param2Line->setVisible(0);
      param2Label->setText("");
      param2Line->setText("");
    }
  else if (dist_type == 4)
    {
      param1Line->setVisible(1);
      param1Line->setText("3");
      param1Label->setText("Fixed degree:");
      param2Line->setVisible(0);
      param2Label->setText("");
      param2Line->setText("");
    }

}

void MainWindow::createGridGroupBox()
// Creates the main input forms and their labels
{

  // Define text boxes
  numrunsLine = new QLineEdit();
  numrunsLine->setAlignment(Qt::AlignRight);
  numnodesLine = new QLineEdit();
  numnodesLine->setAlignment(Qt::AlignRight);
  param1Line = new QLineEdit();
  param1Line->setAlignment(Qt::AlignRight);
  param2Line = new QLineEdit();
  param2Line->setAlignment(Qt::AlignRight);
  pzeroLine = new QLineEdit();
  pzeroLine->setAlignment(Qt::AlignRight);
  rzeroLine = new QLineEdit();
  rzeroLine->setAlignment(Qt::AlignRight);

  // Define all of the labels, in order of appearance

  QLabel *numrunsLabel = new QLabel(tr("Number of runs:"));
  QLabel *numnodesLabel = new QLabel(tr("Number of nodes:"));
  QLabel *distLabel = new QLabel(tr("Degree distribution:"));
  param1Label = new QLabel(tr("Parameter 1 value:"));
  param2Label = new QLabel(tr("Parameter 2 value:"));
  QLabel *pzeroLabel = new QLabel(tr("Patient zero count:"));
  QLabel *rzeroLabel = new QLabel(tr("Basic reproductive ratio:"));

  // Build dropdown box

  distBox = new QComboBox;

  distBox->addItem("Poisson");
  distBox->addItem("Exponential");
  distBox->addItem("Power law");
  distBox->addItem("Urban");
  distBox->addItem("Constant");

  // Initialize layout to parameters for first distribution listed, and listen for changes
  changeParameterLabels(0);
  connect(distBox,SIGNAL(currentIndexChanged (int)), this, SLOT(changeParameterLabels(int))); 

  //Build checkbox

  reuseCheckBox = new QCheckBox(tr("Reuse network"));

  // Put everything together

  gridGroupBox = new QGroupBox(tr("Simulation parameters"));
  QGridLayout *layout = new QGridLayout;

  //First column
  layout->addWidget(numrunsLabel, 0, 0);
  layout->addWidget(numrunsLine, 0, 1);
  layout->addWidget(distLabel, 1, 0);
  layout->addWidget(distBox, 1, 1); 
  layout->addWidget(numnodesLabel, 2, 0);
  layout->addWidget(numnodesLine, 2, 1);
  layout->addWidget(reuseCheckBox,3,0);

  //Second column
  layout->addWidget(rzeroLabel, 0, 3);
  layout->addWidget(rzeroLine, 0, 4);
  layout->addWidget(pzeroLabel, 1, 3);
  layout->addWidget(pzeroLine, 1, 4);
  layout->addWidget(param1Label, 2, 3);
  layout->addWidget(param1Line, 2, 4);
  layout->addWidget(param2Label, 3, 3);
  layout->addWidget(param2Line, 3, 4);
  
  gridGroupBox->setLayout(layout);

  defaultSettings();

}

MainWindow::MainWindow()
// Constructor for the main interface
{
  
  createMenu();
  createHorizontalGroupBox();
  createGridGroupBox();
    
  QWidget* centralWidget = new QWidget(this);


  bigEditor = new QTextEdit();
  bigEditor->setReadOnly (1);
  bigEditor->setPlainText(tr("No output yet"));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setMenuBar(menuBar);
  
  plotArea = new PlotArea(this);
  mainLayout->addWidget(gridGroupBox,Qt::AlignCenter);
  mainLayout->addWidget(plotArea);
  mainLayout->addWidget(bigEditor);
  mainLayout->addWidget(horizontalGroupBox);

  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);

  setWindowTitle(tr("EpiFire GUI"));

}


void MainWindow::makeHistogram(int* data_series, int num_runs, int pop_size)
{

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

    const int numValues = 20; //Number of bins

    //Perform the actual binning

    int histogramValues[numValues]={0};
    int bin;

    for (int i=0; i<num_runs; i++)
      {
	bin=floor((((float)*data_series)/((float)pop_size))*numValues);
	histogramValues[bin]++;
	data_series++;
      }
	
    QwtArray<QwtDoubleInterval> intervals(numValues);
    QwtArray<double> values(numValues);

    double pos = 0.0;
    for ( int i = 0; i < (int)intervals.size(); i++ )
    {
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

void MainWindow::percolationSim()
//Connects the GUI information to the percolation simulator
{
  
  // Get values from textboxes

  int j_max = (numrunsLine->text()).toInt();
  bool reuse_net = reuseCheckBox->isChecked();
  int n = (numnodesLine->text()).toInt();
  double r_zero = (rzeroLine->text()).toDouble();
  double param1 = (param1Line->text()).toDouble();
  double param2 = (param2Line->text()).toDouble();
  int p_zero = (pzeroLine->text()).toInt();
  int index_current_dist=distBox->currentIndex();
  string RunID="1"; // This needs to be updated

  MainWindow::appendOutput("-----------------------------------");
  MainWindow::appendOutput("Simulation running...");

  int dist_size_array[j_max]; //Initiate array that will contain distribution sizes
  int* dist_size_point=dist_size_array;


    //vector< vector<int> > epi_curves = simulate_main(network, j_max, r_zero, p_zero, RunID, dist_size_point);
   vector< vector<int> > epi_curves = simulate_main(j_max, reuse_net,n, r_zero, DistType(index_current_dist), param1, param2, p_zero, RunID, dist_size_point);
  MainWindow::appendOutput("\tDone\n");
  for (unsigned int i =0; i<epi_curves.size(); i++) {
        MainWindow::plotArea->addData(epi_curves[i]);
        QString start_of_line="Epidemic size from run ";
        QString epi_size=QString::number(i+1,10).append(":   ").append(QString::number(dist_size_array[i],10));
      
        MainWindow::appendOutput(start_of_line.append(epi_size));
  }
  makeHistogram(dist_size_point,j_max,n);
  MainWindow::plotArea->replot();
}
