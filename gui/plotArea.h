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

using namespace std;

class PlotArea : public QGraphicsView
{
    Q_OBJECT
    public:

        enum PlotType { STATEPLOT, CURVEPLOT, HISTPLOT, DEGPLOT };
        void setPlotType(PlotType x) { plotType = x; }
        PlotType getPlotType() { return plotType; }

        PlotArea(QWidget* mw, QString l);
        void debugger();
        vector< vector<int> > getData() { return data; }

    public slots:
        void replot();
        void addData(vector<int> X );
        void clearData();
        void clearPlot();
        void saveData();
        void savePlot();
        void setLabel(QString l) {label = l;}
        QString getLabel() {return label;}

    protected:
        void drawEpiCurvePlot();
        void drawHistogram();
        void drawNodeStatePlot();
        void resizeEvent ( QResizeEvent *event );
        void contextMenuEvent(QContextMenuEvent* event);


    private:
        PlotType plotType;
        Axis* xAxis;
        Axis* yAxis;
        vector< vector<int> > data;
        PlotScene* myscene;
        QString label;
        QAction* savePlotAction;
        QAction* saveDataAction;
        
        QList< QGraphicsEllipseItem* > ellipseData;
        int newDataCursor;  // starting position of data that has never been plotted
        int recentDataCursor; // starting position of data from most recent run

        void mouseDoubleClickEvent (QMouseEvent* event) { savePlot(); }
};
#endif
