#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QtGui>
#include <QGraphicsView>
#include <vector>

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
        void clearData() { data.clear(); }
        void saveData();
        void savePicture();

    protected:
        void drawEpiCurvePlot();
        void drawNodeStatePlot();
        void resizeEvent ( QResizeEvent *event );
    private:
        PlotType plotType;
        vector< vector<int> > data;
};
#endif
