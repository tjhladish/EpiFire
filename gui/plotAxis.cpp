#include "plotAxis.h"
#include "plotRegion.h"

Axis::Axis( int type, double min, double max, int nticks, bool force_nticks, bool intLabels) {
    this->type = type;
    this->min = min;
    this->max = max;
    this->nticksPref = nticks;
    if (force_nticks) {
        this->nticks = nticks;
    }
    this->intLabels = intLabels;
    this->minTickLen = 2;
    this->majTickLen = 5;
}


Axis::~Axis() { }

QRectF Axis::boundingRect() const
{

   PlotRegion* myregion = (PlotRegion*) parentItem();

   if (myregion) {
        return(QRectF(0,0,myregion->width(),myregion->height()));
   } else {
        return QRectF(0,0,0,0);
   }
}

void Axis::calculateRange(double minPref, double maxPref) {
    double bin_width;
    double range = maxPref-minPref; // in data coordinates
    // Calculate the order of magnitude (times a constant) that we should use
    // for a good bin width
    if (intLabels and range < 10) { setNumTicks(range); setRange(minPref, maxPref); return;}

    if (! nticksPref || nticksPref <= 0) nticksPref = range < 10 ? range : 10;
    double width_magnitude = pow(10,floor( log(range/nticksPref)/log(10) ) );
    int nice_sizes[5] = {1,2,3,5,10};
    for(int i=0; i<5; i++) { bin_width = nice_sizes[i]*width_magnitude; if (bin_width >= range/nticksPref) break; }

    setNumTicks( ceil(range/bin_width) );
    setRange(minPref, minPref + nticks*bin_width);
}

void Axis::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    painter->setRenderHint(QPainter::Antialiasing, false);

    PlotRegion* myregion = (PlotRegion*) parentItem();
    PlotScene*  myscene = (PlotScene*) scene();
    if(!myregion) return;

    QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    float fontsize = 8;
    QFont font = painter->font();
    font.setPointSize(fontsize);
    painter->setFont(font);

    if (nticks == 0 ) nticks = 2;
    int offset = 0;
    int float_prec = 1; // precision to use when printing floats
    
    double range = max - min;
    double bin_width = range/nticks;
    int ticks = nticks;

    //cerr << "min\tmax\trange\tnticks\tticks\tbin_width\n";
    //cerr << min << "\t" << max << "\t" << range << "\t" << nticks << "\t" << ticks << "\t" << bin_width << endl;

    //Check size of biggest label--is it too big, given tick spacing?
    QFontMetrics fm = painter->fontMetrics();
    int textWidth = 0;

    if ( type == 0) {            // for x-axis
    int x0 = 0;
    int y0 = myregion->height();
    int x1 = myscene->getDataAreaDim().x();
    double ix = (double) (x1-x0)/ticks; // in scene coordinates
        painter->drawLine(x0, y0, x1, y0);
        int decimals = intLabels ? 0 : float_prec; // how many decimal digits to display
        
        //Is there enough room to print all labels, or should we skip some?
        textWidth = fm.width(QString::number(min+bin_width*(ticks-1), 'f', decimals));
        int skip = (1.6*textWidth / ix) + 1; // skip tick labels as necessary to avoid a pile-up
        for (int i=0; i <= ticks; i += skip ) {
            QString label = QString::number(min+bin_width*i,'f',decimals);
            textWidth = fm.width(label);
            painter->drawText(x0 + ix*i - textWidth/2 + 1, y0+15,label);
        }
        for (int i=0; i <= ticks; i++ ) {
            int l = i % skip == 0 ? majTickLen : minTickLen;
            painter->drawLine(x0+ix*i,y0-l,x0+ix*i,y0+l);
        }
    }                            // for y-axis
    else if ( type == 1 ) {
    int x0 = 0;
    int x1 = myregion->width();
    int y0 = myscene->getDataAreaDim().y();
    int y1 = 0;
    double iy = (double) (y1-y0)/ticks; //
        painter->drawLine(x0+offset,y0,x0+offset,y1);

        for (int i=0; i <= ticks; i++ ) {
            int l = majTickLen;
            painter->drawLine(offset+x0-l,y0+iy*i,offset+x0+l,y0+iy*i);
        }

        for (int i=0; i <= ticks; i++ ) {
            double num_val = min + bin_width*i;
            QString value = QString::number(num_val,'g',float_prec);
            if ( max < 10000 && intLabels ) { value = QString::number(num_val,'f',0); }
            int textHeight = fm.height() - 4;
            painter->drawText(offset+x0-7-fm.width(value),y0+iy*i + textHeight/2,value);
        }

        QColor line_col(200,200,200,100);
        QPen pen(line_col, 0.1,Qt::DotLine);
        painter->setPen(pen);
        for (int i=0; i <= ticks; i++ )
            painter->drawLine(offset+x0-5,y0+iy*i,x1,y0+iy*i);
    }
}
