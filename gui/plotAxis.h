#ifndef PLOT_AXES_H
#define PLOT_AXES_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QFlags>
#include <math.h>

class Axis : public QGraphicsItem
{
    public:
        Axis( int type, float min, float max, int nticks);
        ~Axis();
        void setRange(float a, float b) { min=a; max=b; }
        void setXLabel(QString t) { xlabel = t; }
        void setYLabel(QString t) { ylabel = t; }
        void setTitle(QString  t) { title =  t; }
        void setNumTicks(int x)  { nticks = x; }

    protected:
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        int type;
        float min;
        float max;
        int nticks;
        QString xlabel;
        QString ylabel;
        QString title;

};
#endif
