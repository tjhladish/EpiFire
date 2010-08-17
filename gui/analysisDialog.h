#ifndef ANALYSISDIALOG
#define ANALYSISDIALOG

#include <QDialog>
#include "mainWindow.h"
#include "backgroundthread.h"
#include "../src/Network.h"

class MainWindow;
class BackgroundThread;

class AnalysisDialog: public QDialog {
    Q_OBJECT

    friend class MainWindow;

    public:
        enum DialogType {NETWORK, RESULTS};
        AnalysisDialog(MainWindow* w, DialogType d, QString title);

        void setDialogType(DialogType t) { dialogType=t; }

    public slots:
        // Network analysis slots
        void analyzeNetwork();
        void generate_comp_thread();
        void calculateComponentStats();
        void generate_trans_thread();
        void calculateTransitivity();
        void generate_dist_thread();
        void calculateDistances();

        // Results analysis slots
        void analyzeResults();
        void updateResultsAnalysis();
    
    signals:

    protected:

    private:
        DialogType dialogType;
        MainWindow* mw;
        Network* network;
        //BackgroundThread* backgroundThread;

        // Network analysis dialog
        void createNetworkAnalysis();
        void _addNetAnalysisRow(QGridLayout* layout, QString label, QLineEdit* box, QPushButton* button = NULL);

        QLineEdit* nodeCountEdit;
        QLineEdit* edgeCountEdit;
        QLineEdit* meanDegreeEdit;
        QLineEdit* componentCountEdit;
        QLineEdit* maxComponentSizeEdit;
        QLineEdit* transitivityEdit;
        QLineEdit* diameterEdit;
        QLineEdit* meanDistanceEdit;

        QPushButton* componentButton1;
        QPushButton* componentButton2;
        QPushButton* transitivityButton;
        QPushButton* diameterButton;
        QPushButton* meanDistanceButton;

        PlotArea* degDistPlot;

        // Results analysis dialog
        void createResultsAnalysis();
        void _addResultsAnalysisRow(QGridLayout* layout, QString label, QLineEdit* n, QLineEdit* min, QLineEdit* max, QLineEdit* mean, QLineEdit* sd);

        int find_epi_threshold(vector<int> data);
        QLineEdit* thresholdEdit;
        
        QLineEdit* outNEdit;
        QLineEdit* outMeanEdit;
        QLineEdit* outSDEdit;
        QLineEdit* outMinEdit;
        QLineEdit* outMaxEdit;
        
        QLineEdit* epiNEdit;
        QLineEdit* epiMeanEdit;
        QLineEdit* epiSDEdit;
        QLineEdit* epiMinEdit;
        QLineEdit* epiMaxEdit;
        
        QLineEdit* allNEdit;
        QLineEdit* allMeanEdit;
        QLineEdit* allSDEdit;
        QLineEdit* allMinEdit;
        QLineEdit* allMaxEdit;



};

#endif
