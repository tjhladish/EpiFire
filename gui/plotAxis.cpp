#include "plotAxis.h"

Axis::Axis( int type, float min, float max, int nticks) {
    this->type = type;
    this->min = min;
    this->max = max;
    this->nticks = nticks;
}


Axis::~Axis() { }

QRectF Axis::boundingRect() const
{

    return(QRectF(0,0,scene()->width()+10,scene()->height()+10));

}


void Axis::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    float fontsize = 8;
    QFont font("Helvetica",8);
    painter->setFont(font);

    if (nticks == 0 ) nticks = 2;
    int x0 = 0;
    int y0 = scene()->height();
    int x1 = scene()->width();
    int y1 = 0;
    int offset = 8;
    int ao = 10;                 // axis offset, so axes don't touch
    int margin = 35;

    if ( type == 0) {            // shift x-axis down and to the right
        x0 += margin;
        y0 += ao;
    }

    float range = (max-min);
    float expbase = pow(10,floor( log(range/nticks)/log(10) ) );
    float b;
    for(int i=0; i<10; i+=1) { b=i*expbase; if (b>range/nticks) break; }

    float ticks = range/b;
    float ix = (x1-x0)/ticks;
    float iy = (y1-y0)/ticks;

    if ( b == 0 ) return;

    if ( type == 0) {            // for x-axis
        painter->drawLine(x0, y0, x1, y0);
        for (int i=0; i <= ticks; i++ ) painter->drawLine(x0+ix*i,y0-5,x0+ix*i,y0+5);
        for (int i=0; i <= ticks; i++ ) painter->drawText(x0+ix*i,y0+10,QString::number(min+b*i,'f',2));
    }                            // for y-axis
    else if ( type == 1 ) {
        painter->drawLine(x0+offset,y0,x0+offset,y1);

        for (int i=0; i <= ticks; i++ ) {
            painter->drawLine(offset+x0-5,y0+iy*i,offset+x0+5,y0+iy*i);
        }

        for (int i=0; i <= ticks; i++ ) {
            QString value = QString::number(min+b*i,'g',2);
            if ( max < 10000 ) { value = QString::number(min+b*i,'f',1); }
            painter->drawText(offset+x0+5,y0+iy*i,value);
        }

        QColor line_col(200,200,200,100);
        QPen pen(line_col, 0.1,Qt::DotLine);
        painter->setPen(pen);
        for (int i=0; i <= ticks; i++ )
            painter->drawLine(offset+x0-5,y0+iy*i,x1,y0+iy*i);
    }
    //painter->drawText(posX-10,posY-3,QString( eics[i]->sampleName.c_str() ));
}
