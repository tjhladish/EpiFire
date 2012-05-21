#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define QT_USE_FAST_CONCATENATION
#define PROG(x) cerr << x << endl

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QSizePolicy>

#include "debug.h"
#include "plotView.h"
#include "graphwidget.h"
#include "backgroundthread.h"
#include "analysisDialog.h"
#include "../src/Network.h"
#include "../src/Simulator.h"
#include "../src/Percolation_Sim.h"
#include "../src/ChainBinomial_Sim.h"

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

//Set default parameter values

const QString default_num_runs="1";
const QString default_network_size="10000";
const QString default_P0="1";
const QString default_T ="0.1";
const QString default_infectious_pd ="5";

const QString default_poi_param1 = "5.0";
const QString default_exp_param1 = "0.3";
const QString default_pow_param1 = "1.5";
const QString default_pow_param2 = "10.0";
const QString default_con_param1 = "5";
const QString default_smw_param1 = "6";
const QString default_smw_param2 = "0.01";

const QString generateNetMsg = "Click Generate network to begin";
const QString loadNetMsg = "Click Import edge list to begin";
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
class PlotView;
class GraphWidget;
class BackgroundThread;
class AnalysisDialog;

void makeReadonly(QLineEdit*);
void makeEditable(QLineEdit*);
QString frequencyFormat(double numerator, double denominator);

//Define public and private functions and slots for 'MainWindow' class
class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class BackgroundThread;
    friend class AnalysisDialog;

        public:
        MainWindow();

        Network* network;
        Simulator* simulator;
        GraphWidget* networkPlot;
        BackgroundThread* backgroundThread;
        AnalysisDialog* netAnalysisDialog;
        AnalysisDialog* resultsAnalysisDialog;
        QTextEdit* logEditor;
        QProgressDialog* progressDialog;

        enum DistType  { POI, EXP, POW, URB, CON, SMW};
        int rep_ct;
        vector< vector<Node*> > netComponents;
        void updateProgress(int x);

    signals:
        void progressUpdated(int);
    
    public slots:
        // Parameter slots 
        void changeNetSource(int source);
        void defaultNetworkParameters();
        void changeNetworkParameters(int dist_type);
        void changeSimType(int type);
        bool validateParameters(); // currently only transmissibility
        void updateRZero();
        void defaultSettings();

        // Network creation/deletion slots
        void generate_network_thread();
        bool generate_network();
        bool connect_network (Network* net, DistType dist, double param1, double param2);
        void netDoneUpdate(bool success);
        void readEdgeList();
        void saveEdgeList();
        void clear_network();
        void removeMinorComponents();

        // Simulation slots
        void simulatorWrapper();
        void clear_data();
        void generate_sim_thread();

        // Plot update slots
        void updateNetworkPlot();
        void updatePlotMenuFlags();
        void showHideStatePlot();
        void showHideEpiCurvePlot();
        void showHideHistPlot();
        void plotNetwork();

        // Misc
        void appendOutput(QString);
        void appendOutputLine(QString);
        void stopBackgroundThread();
        void resetCursor();
        void enableCentralWidget();
        void disableCentralWidget();
 
    protected:

    private:

        bool networkBusy;
        bool simulatorBusy;
        
        // Menu bar
        QMenu* fileMenu;
        QAction* exitAction;
        QAction* openAction;
        
        QMenu* plotMenu;
        QAction* showStatePlot;
        QAction* showEpiPlot;
        QAction* showHistPlot;

        // Central widget
        QWidget* centralWidget;
        QGroupBox* mainBox;
        QMenuBar* menuBar;
        void createMenu();
        QWidget* leftBox;
        QTabWidget* tabWidget;
        QSplitter* rightBox;

        // Network settings
        QWidget* networkSettingsGroupBox;
        void createNetworkSettingsBox();
        QLabel* netsourceLabel;
        QComboBox* netsourceBox;
        QLabel* distLabel;
        QComboBox* distBox;

        QLabel* param1Label;
        QLineEdit* poiLambdaLine;
        QLineEdit* expBetaLine;
        QLineEdit* powAlphaLine;
        QLineEdit* conValueLine;
        QLineEdit* smwKLine;

        QLabel* param2Label;
        QLineEdit* powKappaLine;
        QLineEdit* smwBetaLine;

        QLabel* netfileLabel;
        QLineEdit* netfileLine;
        QLineEdit* numnodesLine;


        // Simulator settings
        QWidget* simulatorSettingsGroupBox;
        void createSimulatorSettingsBox();
        QLabel* simLabel;
        QComboBox* simBox;
        QLabel* infectiousPeriodLabel;
        QLineEdit* pzeroLine;
        QLineEdit* transLine;
        QLineEdit* infectiousPeriodLine;
        QLineEdit* numrunsLine;
        int J, J_max; // What run we're on
        
        // Control panel
        QGroupBox* controlButtonsGroupBox;
        void createControlButtonsBox();
        QPushButton* clearNetButton;
        QPushButton* defaultSettingsButton;
        QPushButton* loadNetButton;
        QPushButton* generateNetButton;
        QPushButton* clearDataButton;
        //QPushButton* helpButton;
        QPushButton* analyzeNetButton;
        QPushButton* runSimulationButton;
        QCheckBox* retainDataCheckBox;

        // Predictions panel
        QGroupBox* predictionsGroupBox;
        void createPredictionsBox();
        QLineEdit* rzeroLine;
        QLineEdit* maPredictionLine;
        QLineEdit* netPredictionLine;

        // Main plots
        void createPlotPanel();
        PlotView* epiCurvePlot;
        PlotView* statePlot;
        PlotView* histPlot;
        void addStateData();

        // Helper functions
        double calculate_T_crit();
        double getPercTransmissibility();
        double convertR0toT(double R0);
        double convertTtoR0(double T);
        double convertTtoTCB (double T, int d);
        double convertTCBtoT (double TCB, int d);
        double maExpectedSize(double R0, double lower_bound, double upper_bound);
        double netExpectedSize(double T, double P0_frac);
        
        int percent_complete(int current, double predicted);
};
#endif
