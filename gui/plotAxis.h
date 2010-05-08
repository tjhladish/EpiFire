#ifndef PLOT_AXES_H
#define PLOT_AXES_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <math.h>

class Axis : public QGraphicsItem
{
    public:
        Axis( int type, float min, float max, int nticks);
        ~Axis();

    protected:
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        int type;
        float min;
        float max;
        int nticks;

};
#endif
