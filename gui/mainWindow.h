#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QMainWindow>
#include <iostream>
#include "guiSim.h"
//#include "main_histexample.h"
#include <math.h>
#include "plotArea.h"

//Temporary includes for making the histogram
#include <stdlib.h>
#include <qapplication.h>
#include <qpen.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval_data.h>
#include "histogram.h"

//Set default values that are not related to distribution

const QString default_R0="1.5";
const QString default_num_runs="1";
const QString default_network_size="10000";
const QString default_P0="1";

// Forward definitions of classes
class QMainWindow;
class QAction;
class QMainWindowButtonBox;
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

//Define public and private functions and slots for 'MainWindow' class
class MainWindow : public QMainWindow
{
     Q_OBJECT

 public:
  MainWindow();
  void appendOutput(QString); 
   
  Network   *network;
  QTextEdit *bigEditor;

  public slots:

  void percolationSim();
  void changeParameterLabels(int dist_type);
  void changeNetSource(int source);
  void changeSimType(int type);
  void defaultSettings();
  void readEdgeList();

 private:
  
  void createMenu();
  void createHorizontalGroupBox();
  void createGridGroupBox();
  void createFormGroupBox();

  void makeHistogram(int* data_series, int num_runs, int pop_size);

  QLabel *distLabel; 
  QLabel *param1Label;
  QLabel *param2Label;
  QLabel *netsourceLabel;
  QLabel *netfileLabel;
  QLabel *simLabel;


  QMenuBar *menuBar;
  QGroupBox *horizontalGroupBox;
  QGroupBox *gridGroupBox;
  QComboBox *distBox;
  QCheckBox *reuseCheckBox;
  QComboBox *netsourceBox;
  QComboBox *simBox;

  QPushButton *buttons[4];
  QPushButton* clearnetButton;
  QPushButton* loadNetButton;

  QMainWindowButtonBox *buttonBox;

  // Define textboxes and other main menu items

  QLineEdit *numrunsLine;
  QLineEdit *numnodesLine;
  QLineEdit *param1Line;
  QLineEdit *param2Line;
  QLineEdit *pzeroLine;
  QLineEdit *rzeroLine;
  QLineEdit *netfileLine;

  
  PlotArea* plotArea;
  
  QMenu *fileMenu;
  QAction *exitAction;
  QAction *openAction;
};

 #endif

