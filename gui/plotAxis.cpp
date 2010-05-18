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
    //QFlags<QPainter::RenderHints> savedHints( painter->renderHints() );
    //QPainter::RenderHints savedHints( painter->renderHints() );
    painter->setRenderHint(QPainter::Antialiasing, false);

    QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    float fontsize = 8;
    QFont font("Helvetica",fontsize);
    painter->setFont(font);

    if (nticks == 0 ) nticks = 2;
    int x0 = 0;
    int y0 = scene()->height();
    int x1 = scene()->width();
    int y1 = 0;
    int offset = 8;
    int float_prec = 1; // precision to use when printing floats
    
    /*
    float range = (max-min);
    float expbase = pow(10,floor( log(range/nticks)/log(10) ) );
    float b;
    for(int i=0; i<10; i+=1) { b=i*expbase; if (b>range/nticks) break; }

    float ticks = range/b;
    */
   
    double range, bin_width;
    int ticks;
    if (force_nticks) {
        range = max - min;
        bin_width = range/nticks;
        ticks = nticks;
    } else {
        range = ceil(max-min);
        bin_width = (int) range/nticks;
        ticks = ((int) range % nticks == 0) ? nticks : nticks + 1;
    }

    double ix = (double) (x1-x0)/ticks;
    double iy = (double) (y1-y0)/ticks;

    //if ( b == 0 ) return;
    //Check size of biggest label--is it too big, given tick spacing?
    QFontMetrics fm = painter->fontMetrics();

    if ( type == 0) {            // for x-axis
        painter->drawLine(x0, y0, x1, y0);
        for (int i=0; i <= ticks; i++ ) painter->drawLine(x0+ix*i,y0-5,x0+ix*i,y0+5);
        int decimals = force_nticks ? float_prec : 0; // how many decimal digits to display
        
        //Is there enough room to print all labels, or should we skip some?
        int maxLabelWidth = fm.width(QString::number(min+bin_width*(ticks-1), 'f', decimals));
        int skip = ix > maxLabelWidth*1.6 ? 1 : 2; // print every other one if spacing is small
        for (int i=0; i <= ticks; i += skip ) painter->drawText(x0+ix*i,y0+10,QString::number(min+bin_width*i,'f',decimals));
    }                            // for y-axis
    else if ( type == 1 ) {
        painter->drawLine(x0+offset,y0,x0+offset,y1);

        for (int i=0; i <= ticks; i++ ) {
            painter->drawLine(offset+x0-5,y0+iy*i,offset+x0+5,y0+iy*i);
        }

        for (int i=0; i <= ticks; i++ ) {
            double num_val = min + bin_width*i;
            QString value = QString::number(num_val,'g',float_prec);
            if ( max < 10000 && max > 10 && ! force_nticks ) { value = QString::number(num_val,'f',0); }
            painter->drawText(offset+x0+5,y0+iy*i,value);
        }

        QColor line_col(200,200,200,100);
        QPen pen(line_col, 0.1,Qt::DotLine);
        painter->setPen(pen);
        for (int i=0; i <= ticks; i++ )
            painter->drawLine(offset+x0-5,y0+iy*i,x1,y0+iy*i);
    }
    //painter->drawText(posX-10,posY-3,QString( eics[i]->sampleName.c_str() ));
    //painter->setRenderHint((QPainter::RenderHints) savedHints);
}
