#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define QT_USE_FAST_CONCATENATION

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QSizePolicy>

#include "plotArea.h"
#include "graphwidget.h"
#include "../src/Network.h"
#include "../src/Simulator.h";
#include "../src/Percolation_Sim.h"
#include "../src/ChainBinomial_Sim.h"

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

//Set default values that are not related to distribution

const QString default_num_runs="1";
const QString default_network_size="10000";
const QString default_P0="10";
const QString default_T ="0.1";
const QString default_infectious_pd ="5";

const QString generateNetMsg = "Click Generate network to begin";
const QString clearedNetMsg = "Network deleted";
const QString clearedDataMsg = "All data deleted";
const QString simulateMsg = "Click Run simulation to generate data";
const QString saveDataMsg = "Right-click on plot to save image or data";
const QString busyNetMsg = "Generating network topology . . .";
const QString busySimMsg = "Running simulation";
const QString simDoneMsg = "Simulation complete";

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
class GraphWidget;

//Define public and private functions and slots for 'MainWindow' class
class MainWindow : public QMainWindow
{
    Q_OBJECT

        public:
        MainWindow();
        void appendOutput(QString);
        void appendOutputLine(QString);

        Network* network;
        Simulator* simulator;
        GraphWidget* graphWidget;
        QDialog* netAnalysisDialog;
        QTextEdit* logEditor;

        enum DistType  { POI, EXP, POW, URB, CON};
        int rep_ct;

    public slots:

        void simulatorWrapper();
        void changeParameterLabels(int dist_type);
        void changeNetSource(int source);
        void changeSimType(int type);
        void updateRZero();
        void defaultSettings();
        void readEdgeList();
        void clear_network();
        void clear_data();
        void generate_network();
        bool connect_network (Network* net, DistType dist, double param1, double param2);
        void saveEdgeList();
        void showNetworkPlot();
        void showNetworkAnalysis();
        void calculateTransitivity();
        void calculateDiameter();
        void calculateMeanDistance();
 
    protected:

    private:

        void createMenu();
        void createSettingsBox();
        void createNetworkSettingsBox();
        void createSimulatorSettingsBox();
        void createControlButtonsBox();
        void createNetworkAnalysis();
        void runSimulation(int j_max, int patient_zero_ct, string RunID);
        double guessEpiSize(double R0, double P0_frac, double guess);

        void makeReadonly(QLineEdit* lineEdit);
        void addStateData();

        QWidget* centralWidget;

        QLabel* distLabel;
        QLabel* param1Label;
        QLabel* param2Label;
        QLabel* netsourceLabel;
        QLabel* netfileLabel;
        QLabel* simLabel;
        QLabel* infectiousPeriodLabel;

        QGroupBox* mainBox;
        QGroupBox* leftBox;
        QSplitter* rightBox;

        QMenuBar* menuBar;
        QGroupBox* settingsGroupBox;
        QGroupBox* networkSettingsGroupBox;
        QGroupBox* simulatorSettingsGroupBox;
        QGroupBox* controlButtonsGroupBox;

        QComboBox* distBox;
        QComboBox* netsourceBox;
        QComboBox* simBox;

        QCheckBox* retainDataCheckBox;
    
        QPushButton* clearNetButton;
        QPushButton* defaultSettingsButton;
        QPushButton* loadNetButton;
        QPushButton* generateNetButton;
        QPushButton* clearDataButton;
        QPushButton* helpButton;
        QPushButton* runSimulationButton;
     
        QMainWindowButtonBox* buttonBox;

        double calculate_T_crit();
        double convertR0toT(double R0);
        double convertTtoR0(double T);
        double convertTtoTCB (double T, int d);
        double convertTCBtoT (double TCB, int d);
        int percent_complete(int current, double predicted);


        // Define textboxes and other main menu items

        QLineEdit* numrunsLine;
        QLineEdit* numnodesLine;
        QLineEdit* param1Line;
        QLineEdit* param2Line;
        QLineEdit* pzeroLine;
        QLineEdit* transLine;
        QLineEdit* rzeroLine;
        QLineEdit* infectiousPeriodLine;
        QLineEdit* netfileLine;

        PlotArea* epiCurvePlot;
        PlotArea* statePlot;
        PlotArea* histPlot;
        PlotArea* degDistPlot;

        //QWidget* dockWidget1;
        //QWidget* dockWidget2;

        QDoubleValidator* probValidator;
        QMenu* fileMenu;
        QMenu* plotMenu;
        QAction* exitAction;
        QAction* openAction;
        QProgressDialog* simProgress;
        
        QLineEdit* nodeCountEdit;
        QLineEdit* edgeCountEdit;
        QLineEdit* meanDegreeEdit;
        QLineEdit* componentCountEdit;
        QLineEdit* maxComponentSizeEdit;
        QLineEdit* transitivityEdit;
        QLineEdit* diameterEdit;
        QLineEdit* meanDistanceEdit;

        QPushButton* transitivityButton;
        QPushButton* diameterButton;
        QPushButton* meanDistanceButton;

};
#endif
