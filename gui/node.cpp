#include "node.h"
#include <iostream>
#include <QDebug>

GNode::GNode(QGraphicsItem* parent, QGraphicsScene *scene):QGraphicsItem(parent) {
    if (scene) scene->addItem(this);
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setAcceptHoverEvents(true);
    setVisible(true);

    setZValue(1);
    setFixedPosition(false);
    setPos(0,0);

    _boxHeight=10;
    _boxWidth=10;
    _nodeSize=5;
    
    _brush = QBrush(Qt::black);
	_graph = NULL;
}

GNode::~GNode() {}

QList<GEdge*> GNode::edgesIn() { 
		QList<GEdge*>elist;
		foreach(GEdge* e, edgeList) if (e->destGNode() == this ) elist << e;
		return elist;
}

QList<GEdge*> GNode::edgesOut() { 
		QList<GEdge*>elist;
		foreach(GEdge* e, edgeList) if (e->sourceGNode() == this ) elist << e;
		return elist;
}

void GNode::addGEdge(GEdge *edge) { 
    edgeList << edge; edge->adjust(); 
}

QList<GEdge*> GNode::findConnectedGEdges(GNode* other) {
	QList<GEdge*>elist;
	foreach(GEdge* e, edgeList) if (e->sourceGNode() == other || e->destGNode() == other ) elist << e;
    return elist;
}


bool GNode::setNewPos(float x, float y) {
	if (isFixedPosition()) return false;
	setPos(x,y); 
	return true;
}


QRectF GNode::boundingRect() const {
    return QRectF(-_boxWidth/2, -_boxHeight/2, _boxWidth, _boxHeight);
}

void GNode::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *) {
    int numNodes = getGraphWidget()->getNumNodes();
    int nodesize = 10/(numNodes > 10 ? log10(numNodes) : 1);
	_boxWidth=nodesize;
	_boxHeight=nodesize;
    painter->setBrush(QBrush(_brush));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-nodesize/2,-nodesize/2,nodesize,nodesize);
	setBoundingBox(_boxWidth+1,_boxHeight+1);
}


void GNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) {
    emit(nodeDoubleClicked(this));
}

void GNode::mousePressEvent(QGraphicsSceneMouseEvent *) {
	scene()->clearSelection();
    emit(nodePressed(this));
}


void GNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mouseMoveEvent(event);
    foreach (GEdge *edge, edgeList) edge->adjust();
    getGraphWidget()->forceLayout(1);
    scene()->update();
}

QVariant GNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);

}

void GNode::removeGEdge(GEdge* edge) { 
	edgeList.removeAll(edge);
}


void GNode::setGraphWidget(GraphWidget *g) { _graph = g; }

GraphWidget* GNode::getGraphWidget() { return _graph; }

