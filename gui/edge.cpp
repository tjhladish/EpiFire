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


void GEdge::adjust() {
    if(_source) sourcePoint = mapFromItem(_source, 0, 0);
    if(_dest) destPoint = mapFromItem(_dest, 0, 0);
}

double GEdge::length() { 
	return QLineF(sourcePoint,destPoint).length();
}

QRectF GEdge::boundingRect() const {
    if (!_source || !_dest) return QRectF();
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
    GNode* tmpGNode = _source;
    _source = _dest;
    _dest = tmpGNode;
}

void GEdge::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * ) {
	reverseDirection();
	update();
}

void GEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    if (!_source || !_source->isVisible()) return;
    if (!_dest   || !_dest->isVisible()) return;

    QPen pen;
    int numNodes = getGraphWidget()->getNumNodes();
    pen.setWidthF(numNodes > 10 ? 1.0/log10(numNodes) : 1);
    pen.setColor(QColor(0,0,0,80));
    painter->setPen(pen);
    sourcePoint = mapFromItem(_source, 0, 0);
    destPoint = mapFromItem(_dest,   0, 0);
    QPainterPath path; 
    path.moveTo(sourcePoint);
    path.lineTo(destPoint);
	painter->drawPath(path);

	_shape=path;
}
