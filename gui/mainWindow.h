#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QApplication>
#include <QMainWindow>

//Temporary includes for making the histogram
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_interval_data.h>
#include "histogram.h"

#include "plotArea.h"
#include "../src/Network.h"
#include "../src/Simulator.h";
#include "../src/Percolation_Sim.h"
#include "../src/ChainBinomial_Sim.h"

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

//Set default values that are not related to distribution

const QString default_R0="1.5";
const QString default_num_runs="1";
const QString default_network_size="10000";
const QString default_P0="1";
const QString default_T ="0.2";

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
        Simulator *simulator;
        QTextEdit *bigEditor;

        enum DistType  { POI, EXP, POW, URB, CON};

    public slots:

        void simulate();
        void changeParameterLabels(int dist_type);
        void changeNetSource(int source);
        void changeSimType(int type);
        void updateRZero();
        void defaultSettings();
        void readEdgeList();
        void clear_network();
        void generate_network();
        void connect_network (Network* net, DistType dist, double param1, double param2);
        void saveEdgeList();

    private:

        void createMenu();
        void createHorizontalGroupBox();
        void createGridGroupBox();
        void createFormGroupBox();
        vector< vector<int> > simulate_main(int j_max, int patient_zero_ct, string RunID, int* dist_size_loc);

        void makeHistogram(int* data_series, int num_runs, int pop_size);

        QLabel *distLabel;
        QLabel *param1Label;
        QLabel *param2Label;
        QLabel *netsourceLabel;
        QLabel *netfileLabel;
        QLabel *simLabel;
        QLabel *infectiousPeriodLabel;

        QMenuBar *menuBar;
        QGroupBox *horizontalGroupBox;
        QGroupBox *gridGroupBox;
        QComboBox *distBox;
        QComboBox *netsourceBox;
        QComboBox *simBox;

        QCheckBox *retainDataCheckBox;

        QPushButton *buttons[4];
        QPushButton* clearnetButton;
        QPushButton* loadnetButton;
        QPushButton* generatenetButton;

        QMainWindowButtonBox *buttonBox;

        // Define textboxes and other main menu items

        QLineEdit *numrunsLine;
        QLineEdit *numnodesLine;
        QLineEdit *param1Line;
        QLineEdit *param2Line;
        QLineEdit *pzeroLine;
        QLineEdit *transLine;
        QLineEdit *rzeroLine;
        QLineEdit *infectiousPeriodLine;
        QLineEdit *netfileLine;

        PlotArea* plotArea;

        QDoubleValidator *probValidator;
        QMenu *fileMenu;
        QAction *exitAction;
        QAction *openAction;
};
#endif
