#include "plotRegion.h"
#include <QDebug>


QRectF PlotRegion::boundingRect() const {
    return(QRectF(0,0,w,h));
}


void PlotRegion::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    qDebug() << "paint " << w << " " << h << " " << x() << " " << y();
    painter->setBrush(brush);
    painter->setPen(Qt::black);
    painter->drawRect(0,0,w,h);

}
