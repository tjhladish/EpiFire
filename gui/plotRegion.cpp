#include "plotRegion.h"
#include "plotScene.h"
#include "plotPoint.h"
#include <QDebug>


QRectF PlotRegion::boundingRect() const {
    return(QRectF(0,0,w,h));
}

void PlotRegion::addPoint(float x, float y, float r) {
    Point* point =  new Point(x,y,r,this,scene());
    point->updatePosition();
}

float PlotRegion::toPlotX( float x ) { 
    PlotScene* _scene = (PlotScene*) scene();
    if(!_scene) return 0;
    return (x-_scene->minX)/(_scene->maxX-_scene->minX)*w;
} 

float PlotRegion::toPlotY( float y ) { 
    PlotScene* _scene = (PlotScene*) scene();
    if(!_scene) return 0;
    return h*(1.0-(y-_scene->minY)/(_scene->maxY-_scene->minY)); 
} 
       

void PlotRegion::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    qDebug() << "paint " << w << " " << h << " " << x() << " " << y();
    painter->setBrush(brush);
    painter->setPen(Qt::black);
    painter->drawRect(0,0,w,h);

    //float r = 5;
    //painter->drawEllipse(toPlotX(5)-r, toPlotY(5)-r, 2*r,2*r);


}
