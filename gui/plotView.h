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
        PlotView(QWidget* mw, QString title, QString xlabel, QString ylabel);
        enum PlotType { STATEPLOT, CURVEPLOT, HISTPLOT, DEGPLOT, RESULTS_HISTPLOT };
        void setPlotType(PlotType x) { plotType = x; }
        PlotType getPlotType() { return plotType; }

        void debugger();
        vector< vector<int> > getData() { return data; }
        int default_nbins(double rangeMin, double rangeMax);
        vector<double> default_minmax();

    signals:
        void epiCurveAxisUpdated(double xRangeMax);

    public slots:
        void replot();
        void addData(vector<int> X );
        void clearData();
        void clearPlot();
        void saveData();
        void savePlot();
        void setRangeMin(double min) { rangeMin=min; replot(); }
        void setRangeMin(QString min) { rangeMin= min.isEmpty()? -1 : min.toDouble(); replot(); }
        void setRangeMax(double max) { rangeMax=max; replot(); }
        void setRangeMax(QString max) { rangeMax= max.isEmpty()? -1 : max.toDouble(); replot(); }
        void setNBins(int bins) { Nbins=bins; replot(); }
        void setNBins(QString bins) { Nbins=bins.toInt(); replot(); }
        void setCutoff(double cut) { cutoff = cut; replot(); }
        void setCutoff(QString cut) { cutoff = cut.isEmpty() ? -1 : cut.toDouble(); replot(); }

    protected:
        void drawEpiCurvePlot();
        void drawHistogram();
        void drawNodeStatePlot();
        void resizeEvent ( QResizeEvent *event );
        void contextMenuEvent(QContextMenuEvent* event);


    private:

        vector< vector<int> > data;

        PlotType plotType;
        double rangeMin;
        double rangeMax;
        int    Nbins;
        double cutoff;
        PlotScene* myscene;
        QAction* savePlotAction;
        QAction* saveDataAction;
        
        int newDataCursor;  // starting position of data that has never been plotted
        int recentDataCursor; // starting position of data from most recent run

        void mouseDoubleClickEvent (QMouseEvent*) { savePlot(); }
};
#endif
