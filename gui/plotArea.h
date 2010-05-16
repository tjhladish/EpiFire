#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QtGui>
#include <QGraphicsView>
#include <vector>
#include <QVector>
#include "plotAxis.h"
#include "plotScene.h"
#include "plotPoint.h"

using namespace std;

class PlotArea : public QGraphicsView
{
    Q_OBJECT
    public:

        enum PlotType { STATEPLOT, EPICURVE, DEGPLOT, HISTPLOT };
        void setPlotType(PlotType x) { plotType = x; }
        PlotType getPlotType() { return plotType; }

        PlotArea(QWidget* mw);
        void debugger();

    public slots:
        void replot();
        void addData(vector<int> X ) { data.push_back(X); }
        void clearData() { data.clear(); recentDataCursor = 0; newDataCursor = 0; ellipseData.clear(); }
        void clearPlot();
        void saveData();
        void savePicture();

    protected:
        void drawEpiCurvePlot();
        void drawNodeStatePlot();
        void resizeEvent ( QResizeEvent *event );

    private:
        PlotType plotType;
        Axis* xAxis;
        Axis* yAxis;
        vector< vector<int> > data;
        
        QList< QGraphicsEllipseItem* > ellipseData;
        int newDataCursor;  // starting position of data that has never been plotted
        int recentDataCursor; // starting position of data from most recent run
};
#endif
