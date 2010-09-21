#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QtGui>
#include <QGraphicsView>
#include <QFile>
#include <QTextStream>
#include <vector>
#include <QVector>
#include "plotAxis.h"
#include "plotScene.h"
#include "plotPoint.h"
#include "plotRegion.h"


using namespace std;
class PlotView : public QGraphicsView
{
    Q_OBJECT

    friend class PlotRegion;
    friend class PlotScene;

    public:

        enum PlotType { STATEPLOT, CURVEPLOT, HISTPLOT, DEGPLOT, RESULTS_HISTPLOT };
        void setPlotType(PlotType x) { plotType = x; }
        PlotType getPlotType() { return plotType; }

        PlotView(QWidget* mw, QString l);
        void debugger();
        vector< vector<int> > getData() { return data; }
        int default_nbins(double rangeMin, double rangeMax);
        vector<double> default_minmax();

    public slots:
        void replot();
        void addData(vector<int> X );
        void clearData();
        void clearPlot();
        void saveData();
        void savePlot();
        void setLabel(QString l) {label = l;}
        QString getLabel() {return label;}
        void setRangeMin(double min) { rangeMin=min; replot(); }
        void setRangeMin(QString min) { rangeMin=min.toDouble(); replot(); }
        void setRangeMax(double max) { rangeMax=max; replot(); }
        void setRangeMax(QString max) { rangeMax=max.toDouble(); replot(); }
        void setNBins(int bins) { Nbins=bins; replot(); }
        void setNBins(QString bins) { Nbins=bins.toInt(); replot(); }
        void setCutoff(double cut) { cutoff = cut; replot(); }
        void setCutoff(QString cut) { cutoff = cut.toDouble(); replot(); }

    protected:
        void drawEpiCurvePlot();
        void drawHistogram();
        void drawNodeStatePlot();
        void resizeEvent ( QResizeEvent *event );
        void contextMenuEvent(QContextMenuEvent* event);


    private:

        vector< vector<int> > data;

        PlotType plotType;
        Axis* xAxis;
        Axis* yAxis;
        double rangeMin;
        double rangeMax;
        int    Nbins;
        double cutoff;
        PlotScene* myscene;
        QString label;
        QAction* savePlotAction;
        QAction* saveDataAction;
        
        int newDataCursor;  // starting position of data that has never been plotted
        int recentDataCursor; // starting position of data from most recent run

        void mouseDoubleClickEvent (QMouseEvent*) { savePlot(); }
};
#endif
