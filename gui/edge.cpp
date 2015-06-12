#include "edge.h"

GEdge::GEdge() {
	_arrowSize = 10;
	_reversable = true;
	_color = Qt::black;
    setVisible(true);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);
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
	update();
}

void GEdge::hoverLeaveEvent ( QGraphicsSceneHoverEvent*) {
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

    QPen pen;
    int numNodes = getGraphWidget()->getNumNodes();
    pen.setWidthF(numNodes > 10 ? 1.0/log10(numNodes) : 1);
    pen.setColor(QColor(0,0,0,80));
    painter->setPen(pen);
    sourcePoint = mapFromItem(source, 0, 0);
    destPoint = mapFromItem(dest,   0, 0);
    QPainterPath path; 
    path.moveTo(sourcePoint);
    path.lineTo(destPoint);
	painter->drawPath(path);

	_shape=path;
}
