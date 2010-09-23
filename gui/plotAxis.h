#ifndef PLOT_AXES_H
#define PLOT_AXES_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QFlags>
#include <math.h>
#include <iostream>

using namespace std;

class Axis : public QGraphicsItem
{
    public:
        Axis( int type, double min, double max, int nticks, bool force_nticks, bool intLabels);
        ~Axis();
        void setRange(double a, double b) { min=a; max=b; } // may change upon painting
        void calculateRange(double a, double b); 
        double getMin() {return min;}
        double getMax() {return max;}
        void setNumTicks(int x)  { nticks = x; }
        void preferedNumTicks(int x) {nticksPref = x; }
        void forceNumTicks(bool t) { force_nticks = t; }
        bool numTicksForced() { return force_nticks; }
        void useIntLabels(bool t) {intLabels = t;}
        int getMajTickLen() {return majTickLen;}
        int getMinTickLen() {return minTickLen;}


    protected:
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    private:
        int type;
        double min;
        double max;
        int nticks;
        int nticksPref;
        bool force_nticks;
        int majTickLen;
        int minTickLen;
        bool intLabels;

};
#endif
