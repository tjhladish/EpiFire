#ifndef PLOT_POINT_H
#define PLOT_POINT_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QFlags>
#include <math.h>

class Point : public QGraphicsItem
{
    public:
        enum { Type = UserType + 1 };
        int type() const { return Type; }

        Point( float nx, float ny, float nr, QGraphicsItem* parent=0, QGraphicsScene* scene=0):QGraphicsItem(parent,scene) { x=nx; y=ny; r=nr; brush=QBrush(Qt::black); };
        void setBrush(QBrush& x) { brush = x; }
        void updatePosition(); 


    protected:
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        float r;
        float x;
        float y;
        QBrush brush;

};
#endif
