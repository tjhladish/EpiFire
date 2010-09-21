#ifndef PLOT_REGION_H
#define PLOT_REGION_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QFlags>
#include "plotScene.h"
#include <math.h>


class PlotRegion : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_INTERFACES( QGraphicsItem )

    friend class PlotScene;
    friend class PlotView;
    
    public:
        enum { Type = UserType + 1 };
        int type() const { return Type; }
        PlotRegion(int iw, int ih, QGraphicsScene* scene=0):QGraphicsRectItem(0,scene) { w=iw; h=ih; }

        void setWidthHeight(int iw, int ih) { w=iw; h=ih; }
        int width()  { return w; }
        int height() { return h; }
        float toPlotX( float x );
        float toPlotY( float y );
        void addPoint( float x, float y, float r);

    protected:
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        int w;
        int h;
        //QBrush brush;

};

#endif
