#ifndef PLOT_REGION_H
#define PLOT_REGION_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QFlags>
#include <math.h>


class PlotRegion : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem )

    public:
        enum { Type = UserType + 1 };
        int type() const { return Type; }
        PlotRegion(int iw, int ih, QGraphicsScene* scene=0):QGraphicsRectItem(0,scene) { w=iw; h=ih; }

        void setWidthHeight(int iw, int ih) { w=iw; h=ih; }
        int width()  { return w; }
        int height() { return h; }
        void setXrange(float a, float b) { minX=a; maxX=b; }
        void setYrange(float a, float b) { minY=a; maxY=b; }
        float getMinX() { return minX; }
        float getMinY() { return minY; }
        float getMaxX() { return maxX; }
        float getMaxY() { return maxY; }
        float toPlotX( float x ) { return (x-minX)/(maxX-minX)*width(); } 
        float toPlotY( float y ) { return height()*(1.0-(y-minY)/(maxY-minY)); } 

    protected:
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        int w;
        int h;
        float xmargin;
        float ymargin;
        float minX;
        float minY;
        float maxX;
        float maxY;
        QBrush brush;

};

#endif
