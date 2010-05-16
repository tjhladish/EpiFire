#ifndef PLOTSCENE_H
#define PLOTSCENE_H

#include <QtGui>
#include <QGraphicsScene>


class PlotScene : public QGraphicsScene
{
    Q_OBJECT

    public:
     PlotScene(QObject* parent):QGraphicsScene(parent) {};

     void setXrange(float a, float b) { minX=a; maxX=b; }
     void setYrange(float a, float b) { minY=a; maxY=b; }

     float getMinX() { return minX; }
     float getMinY() { return minY; }
     float getMaxX() { return maxX; }
     float getMaxY() { return maxY; }
     float toPlotX( float x ) { return (x-minX)/(maxX-minX)*width(); } 
     float toPlotY( float y ) { return height()*(1.0-(y-minY)/(maxY-minY)); } 
        
   private:
        float xmargin;
        float ymargin;
        float minX;
        float minY;
        float maxX;
        float maxY;

};

#endif
