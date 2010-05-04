#include "plotarea.h"
#include "plot_axes.h"
#include "../src/Utility.h"

PlotArea::PlotArea(QWidget*) { 
    setScene(new QGraphicsScene(this));
    setObjectName("PlotArea");
    setMinimumSize(200,300);
}


void PlotArea::replot() {
    drawPlot();
}

int find_max_val(vector< vector <int> > data) {
    int maxY = 0;
    for( unsigned int i=0; i < data.size(); i++ ) {
        for( unsigned int j=0; j < data[i].size(); j++ ) {
            int test_max = data[i][j];
            if (test_max > maxY) {
                maxY = test_max;
            }
        }
    }
    return maxY;
}

int find_max_idx(vector< vector <int> > data) {
    int maxX = 0;
    for( unsigned int i=0; i < data.size(); i++ ) {
        int test_max = data[i].size() - 1;
        if (test_max > maxX) {
            maxX = test_max;
        }
    }
    return maxX;
}

void PlotArea::drawPlot() {

    // debugging data
/*    data.clear();
    vector<int> dummy;
    data.push_back(dummy);
    for (int i = 0; i < 10; i++) {
        data[0].push_back(i);
    }*/

    if (data.size() == 0 ) return;
    
    scene()->clear();
    int plotW = width() - 50;
    int plotH = height() - 50;
    scene()->setSceneRect(0,0,plotW,plotH);
    //QBrush brush(Qt::red);
    int alpha;
    if (data.size() < 2) {
        alpha = 255;
    } else if (data.size() > 25) {
        alpha = 10;
    } else {
        alpha = 255 / data.size();
    }
    QColor color(0,0,0,alpha);
    QBrush brush(color);
    QPen   pen(color);

    //QColor recent_color(255,0,0,255);
    QColor recent_color = Qt::red;


    //debugging rectangles
/*    int S = 50;
    QColor tc(0,0,0,50);
    QBrush tb(tc);
    QPen   tp(Qt::black);
    scene()->addRect(0,0,S,S,tp,tb);
    scene()->addRect(0,plotH-S,S,S,tp,tb);
    scene()->addRect(plotW-S,0,S,S,tp,tb);
    scene()->addRect(plotW-S,plotH-S,S,S,tp,tb);*/
    
    float axis_multiplier = 1.1; // how much longer should axes be
    float max_val = (float) find_max_val(data) * axis_multiplier;
    int   max_idx = find_max_idx(data) * axis_multiplier;
    float r = 4; // radius of data points
    //int h_padding = 3;
    int margin = 35;
    for( unsigned int i=0; i < data.size(); i++ ) {
        if (i == data.size() - 1) { // make the most recent dataset a different color if it was a single run
            brush.setColor(recent_color);
            pen.setColor(recent_color);
        }
        for( unsigned int j=0; j<data[i].size(); j++ ) {
            float val = data[i][j];
        
            float x  = -r + ((float) j/max_idx * (plotW - margin)) + margin;
            float y  = plotH; // flip coordinate system, since (0,0) is in upper-left
                  y -= plotH * val/max_val + r;
            scene()->addEllipse(x,y,2*r,2*r,pen,brush);
            //qDebug() << data[i][j];
        }
    }
                      
    Axes* x = new Axes(0,0,max_idx*axis_multiplier,9); // args = 0 or 1 for x or y,  min, max, ticks
    scene()->addItem(x);
    
    Axes* y = new Axes(1,0,max_val*axis_multiplier,9);
    scene()->addItem(y);
    
    qDebug() << scene()->width() << " " << scene()->height() << endl;
    //see QGraphicsScene
    
    scene()->update();
    
}


void PlotArea::resizeEvent ( QResizeEvent * event ) {
    replot();	 
}
