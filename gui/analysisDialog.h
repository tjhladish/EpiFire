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
        void setThresholdEdited() {thresholdEdited = true;};
        void reset_epi_threshold(){thresholdEdited = false; thresholdEdit->setText( QString::number(find_epi_threshold()) );};
    
    signals:

    protected:

    private:
        DialogType dialogType;
        MainWindow* mw;
        Network* network;

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

        PlotView* degDistPlot;

        // Results analysis dialog
        void createResultsAnalysis();
        void _addResultsAnalysisRow(QGridLayout* layout, QString label, QLineEdit* all, QLineEdit* out, QLineEdit* epi);

        int find_epi_threshold();
        bool thresholdEdited;
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

        PlotView* resultsHistPlot;

        QLineEdit* nBinsEdit;
        QLineEdit* minRangeEdit;
        QLineEdit* maxRangeEdit;
};

#endif
