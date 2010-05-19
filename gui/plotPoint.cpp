#include "plotPoint.h"
#include "plotScene.h"

QRectF Point::boundingRect() const {
    return(QRectF(-r,-r,2*r,2*r));             
}


void Point::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {

    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-r,-r,2*r,2*r);

}

void Point::updatePosition() { 
      PlotScene* myscene = (PlotScene*) scene();
      setPos( myscene->toPlotX(x), myscene->toPlotY(y)); 
};
