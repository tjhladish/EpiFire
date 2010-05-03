#include <QtGui>
#include <iostream>
#include "dialog.h"
#include "gui_sim.h"


void Dialog::createMenu()
{
  menuBar = new QMenuBar;

  fileMenu = new QMenu(tr("&File"), this);
  exitAction = fileMenu->addAction(tr("E&xit"));
  menuBar->addMenu(fileMenu);

  connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}

void Dialog::appendOutput(QString teststring)
{
  bigEditor->append(teststring);
}

void Dialog::createHorizontalGroupBox()
{
  horizontalGroupBox = new QGroupBox(tr("Control"));
  QHBoxLayout *layout = new QHBoxLayout;

  buttons[0] = new QPushButton("Simulate");
  connect(buttons[0], SIGNAL(clicked()), this, SLOT(simWindow()));

  buttons[1] = new QPushButton("Default Settings");
  buttons[2] = new QPushButton("Help");
  buttons[3] = new QPushButton("Exit");

  connect(buttons[3], SIGNAL(clicked()), this, SLOT(accept()));

  for (int i = 0; i < 4; ++i) {
    layout->addWidget(buttons[i]);
  }
  horizontalGroupBox->setLayout(layout);
}


void Dialog::createGridGroupBox()
{

  // Define text boxes
  numrunsLine = new QLineEdit("1");
  numrunsLine->setAlignment(Qt::AlignRight);
  numnodesLine = new QLineEdit("10000");
  numnodesLine->setAlignment(Qt::AlignRight);
  param1Line = new QLineEdit("5");
  param1Line->setAlignment(Qt::AlignRight);
  param2Line = new QLineEdit("-1");
  param2Line->setAlignment(Qt::AlignRight);
  pzeroLine = new QLineEdit("1");
  pzeroLine->setAlignment(Qt::AlignRight);
  rzeroLine = new QLineEdit("3.5");
  rzeroLine->setAlignment(Qt::AlignRight);


  // Define all of the labels, in order of appearance

  QLabel *numrunsLabel = new QLabel(tr("Number of runs:"));
  QLabel *numnodesLabel = new QLabel(tr("Number of nodes:"));
  QLabel *distLabel = new QLabel(tr("Degree distribution:"));
  QLabel *param1Label = new QLabel(tr("Parameter 1 value:"));
  QLabel *param2Label = new QLabel(tr("Parameter 2 value:"));
  QLabel *pzeroLabel = new QLabel(tr("Patient zero count:"));
  QLabel *rzeroLabel = new QLabel(tr("Basic reproductive ratio:"));

  // Build checkbox

  distBox = new QComboBox;

  reuseCheckBox = new QCheckBox(tr("Reuse network"));
  reuseCheckBox->setChecked(true);

  distBox->addItem("Poisson");
  distBox->addItem("Exponential");
  distBox->addItem("Power law");
  distBox->addItem("Urban");
  distBox->addItem("Constant");


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

  param1Line->text();

  /*

    // OLD FORMAT -- DON'T USE

  gridGroupBox = new QGroupBox(tr("Grid layout"));
  QGridLayout *layout = new QGridLayout;

  for (int i = 0; i < NumGridRows; ++i) {
    labels[i] = new QLabel(tr("Line %1:").arg(i + 1));
    lineEdits[i] = new QLineEdit;
    layout->addWidget(labels[i], i + 1, 0);
    layout->addWidget(lineEdits[i], i + 1, 1);
  }

  smallEditor = new QTextEdit;
  smallEditor->setPlainText(tr("This widget takes up about two thirds of the "
			       "grid layout."));
  layout->addWidget(smallEditor, 0, 2, 4, 1);

  layout->setColumnStretch(1, 10);
  layout->setColumnStretch(2, 20);
  gridGroupBox->setLayout(layout);
  */

}

void Dialog::createFormGroupBox()
{

  // Not currently used -- trying grid method instead, for more flexibility and control

  formGroupBox = new QGroupBox(tr("Simulation Parameters"));
  QFormLayout *layout = new QFormLayout;
  layout->addRow(new QLabel(tr("Size of network:")), new QLineEdit("10000"));
  layout->addRow(new QLabel(tr("Number of seasons:")), new QLineEdit("50"));
  layout->addRow(new QLabel(tr("R0:")), new QLineEdit("3.5"));
  layout->addRow(new QLabel(tr("D:")), new QLineEdit("0.2"));
  layout->addRow(new QLabel(tr("P0:")), new QLineEdit("50"));
  layout->addRow(new QLabel(tr("Network type:")), new QComboBox());
 
  formGroupBox->setLayout(layout);
}




Dialog::Dialog()
{
  
  createMenu();
  createHorizontalGroupBox();
  createGridGroupBox();
  createFormGroupBox();

  bigEditor = new QTextEdit("Output");
  bigEditor->setPlainText(tr("No output yet"));

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->setMenuBar(menuBar);
  
  plotArea = new PlotArea(this);
  mainLayout->addWidget(gridGroupBox,Qt::AlignCenter);
  mainLayout->addWidget(plotArea);
  mainLayout->addWidget(bigEditor);
  mainLayout->addWidget(horizontalGroupBox);
  setLayout(mainLayout);

  setWindowTitle(tr("EpiFire GUI"));

}


void Dialog::simWindow()
{

  // This is supposed to be the function that runs when a simulation is triggered
  
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

  cout << index_current_dist;

  Dialog::appendOutput("Simulation running...");

  vector< vector<int> > epi_curves = simulate_main(j_max, reuse_net,n, r_zero, DistType(index_current_dist), param1, param2, p_zero, RunID);
  Dialog::appendOutput("\tDone\n");
  for (unsigned int i =0; i<epi_curves.size(); i++) {
        Dialog::plotArea->addData(epi_curves[i]);
  }
  Dialog::plotArea->replot();
}
