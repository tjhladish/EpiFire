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
        PlotArea(QWidget* mw);
        
    public slots: 
        void replot();
        void addData(vector<int> X ) { data.push_back(X); } 
        void clearData() { data.clear(); }
        void saveData();
        void savePicture();
        
    protected:
        void drawPlot();
        void resizeEvent ( QResizeEvent *event );
    private:
        vector< vector<int> > data;
};

#endif

