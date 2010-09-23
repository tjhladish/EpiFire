#include "plotPoint.h"
#include "plotScene.h"

QRectF Point::boundingRect() const {
    return(QRectF(-r,-r,2*r,2*r));             
}


void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    //qDebug() << "Point::paint" << x << " " << y <<  " " << r;

    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-r,-r,2*r,2*r);
}

void Point::updatePosition() { 
      PlotRegion* myregion = (PlotRegion*) parentItem();

      if (myregion) {
        setPos( myregion->toPlotX(x), myregion->toPlotY(y)); 
      }
};
