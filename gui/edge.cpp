#include "edge.h"

GEdge::GEdge() {
	_arrowSize = 10;
	_reversable = true;
	_color = Qt::black;
    setVisible(true);
    setFlag(ItemIsSelectable);
	setAcceptsHoverEvents(true);
	setZValue(0);
}

GEdge::~GEdge() {
}

GNode *GEdge::sourceGNode() const {
    return source;
}

void GEdge::setSourceGNode(GNode *node) {
    source = node;
}

GNode *GEdge::destGNode() const {
    return dest;
}

void GEdge::setDestGNode(GNode *node) {
    dest = node;
}

void GEdge::adjust() {
    if(source) sourcePoint = mapFromItem(source, 0, 0);
    if(dest)  destPoint = mapFromItem(dest,   0, 0);
}

double GEdge::angle() { 
	return computeAngle(sourcePoint,destPoint);
}

double GEdge::length() { 
	return QLineF(sourcePoint,destPoint).length();
}

QRectF GEdge::boundingRect() const {
    if (!source || !dest) return QRectF();
    qreal extra = 5;
    QLineF line(sourcePoint,destPoint);
    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(), destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath GEdge::shape() const {
		return _shape;
}

double GEdge::computeAngle(const QPointF& a, const QPointF& b) { 
    return (double) atan2( (b.y()-a.y()), (b.x()-a.x()));
}

float GEdge::computeArrowSize(float flux) { 

	GraphWidget* g=NULL; if ( source ) g = source->getGraphWidget();
	float size = g->getAvgGEdgeLength()*0.25;
	float scale=1; if(g) scale *= g->getGEdgeSizeScale();
	float arrowSize =  size*scale;
    if ( flux > 0 ) { arrowSize += 5*log2(flux); }
	if ( arrowSize < 2)    arrowSize=2;

    return arrowSize;
}

void GEdge::drawArrow(QPainter *painter) {

	if (!source || !source->isVisible()) return;
	if (!dest   || !dest->isVisible()) return;

    sourcePoint = mapFromItem(source, 0, 0);
    destPoint = mapFromItem(dest,   0, 0);
    //qDebug() << "drawArrow: " << sourcePoint << " " << destPoint;
    QPainterPath path; 
    path.moveTo(sourcePoint); path.lineTo(destPoint);
	_shape=path;
	painter->drawPath(path);
}

void GEdge::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
	setSelected(true);
    update();
}


void GEdge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
	setSelected(false);
    update();
}

void GEdge::hoverEnterEvent (QGraphicsSceneHoverEvent*) {
    setToolTip(_note);

	if ( source ) {
		source->setHighlighted(true);
		source->update();
	}

 	if (dest ) {
		dest->setHighlighted(true);
		dest->update();
	}
	update();
}

void GEdge::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) {
	if ( source ) {
		source->setHighlighted(false);
		source->update();
	}

 	if (dest ) {
		dest->setHighlighted(false);
		dest->update();
	}
	update();
}

void GEdge::reverseDirection() {
	GNode* tmpGNode = source;
	source = dest;
	dest = tmpGNode;
}

void GEdge::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * ) {
	reverseDirection();
	update();
}

void GEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
	if (!source || !source->isVisible()) return;
	if (!dest   || !dest->isVisible()) return;

    sourcePoint = mapFromItem(source, 0, 0);
    destPoint = mapFromItem(dest,   0, 0);
    QPainterPath path; 
    path.moveTo(sourcePoint);
    path.lineTo(destPoint);
	painter->drawPath(path);

	_shape=path;
}
