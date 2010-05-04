#ifndef DIALOG_H
#define DIALOG_H

#include <QtGui>
#include <QDialog>
#include <iostream>
#include "gui_sim.h"
//#include "main_histexample.h"
#include <math.h>
#include "plotarea.h"

//Temporary includes for making the histogram
#include <stdlib.h>
#include <qapplication.h>
#include <qpen.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval_data.h>
#include "histogram_item.h"

//Set default values that are not related to distribution

const QString default_R0="1.5";
const QString default_num_runs="1";
const QString default_network_size="10000";
const QString default_P0="1";

// Forward definitions of classes
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
class QComboBox;
class QCheckBox;
class PlotArea;

//Define public and private functions and slots for 'Dialog' class
class Dialog : public QWidget
{
     Q_OBJECT

 public:
  Dialog();
  void appendOutput(QString); 
   
  QTextEdit *bigEditor;

  public slots:

  void percolationSim();
  void changeParameterLabels(int dist_type);
  void defaultSettings();

 private:
  
  void createMenu();
  void createHorizontalGroupBox();
  void createGridGroupBox();
  void createFormGroupBox();

  void makeHistogram(int* data_series, int num_runs, int pop_size);

 
  QLabel *param1Label;
  QLabel *param2Label;

  QMenuBar *menuBar;
  QGroupBox *horizontalGroupBox;
  QGroupBox *gridGroupBox;
  QComboBox *distBox;
  QCheckBox *reuseCheckBox;
  QPushButton *buttons[4];
  QDialogButtonBox *buttonBox;

  // Define textboxes and other main menu items

  QLineEdit *numrunsLine;
  QLineEdit *numnodesLine;
  QLineEdit *param1Line;
  QLineEdit *param2Line;
  QLineEdit *pzeroLine;
  QLineEdit *rzeroLine;

  PlotArea* plotArea;
  
  QMenu *fileMenu;
  QAction *exitAction;
};

 #endif
