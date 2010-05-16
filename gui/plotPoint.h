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

        Point( float nx, float ny, float nr) { x=nx; y=ny; r=nr; };
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
