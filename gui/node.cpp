#include "node.h"
#include <iostream>
#include <QDebug>

GNode::GNode(QGraphicsItem* parent, QGraphicsScene *scene):QGraphicsItem(parent,scene)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setAcceptsHoverEvents(true);
    setVisible(true);
	setHighlighted(false);

    setZValue(1);
    setFixedPosition(false);

    setMolClass(GNode::Unassigned);
    setPos(0,0);
	setDepth(-1);

    _boxHeight=10;
    _boxWidth=10;
    _nodeSize=5;
    
    _brush = QBrush(Qt::white);
	_graph = NULL;
}

GNode::~GNode() {
		//cerr << "Removing " << this << endl;
		//qDebug() << getNote();
}

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

void GNode::calculateForces()
{

    if (_fixedPosition) return;
	if ( !isVisible() ) return;

    if (!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }
    
    // Sum up all forces pushing this item away
    qreal repelx = 0;
    qreal repely = 0;
	qreal attractx = 0;
	qreal attracty = 0;
    int connections = 0;
	//cerr << "Me=" << getId() << endl;
    foreach (QGraphicsItem *item, scene()->items()) {
        GNode *node = qgraphicsitem_cast<GNode *>(item);
        if (!node) continue;
		if ( node == this ) continue;
        if (! node->isVisible()) continue;

        QLineF line(mapFromItem(node, 0, 0), QPointF(0, 0));
        qreal dx = line.dx();
        qreal dy = line.dy();
        double l = sqrt(dx * dx + dy * dy);
        //float overlapX = (this->boundingRect().width() + node->boundingRect().width())/2.0 - l;
        //float overlapY = (this->boundingRect().height() + node->boundingRect().height())/2.0 - l;
        //float overlap = std::max( overlapX, overlapY);
		float weight=1.0;
		repelx += dx/(l*l)*weight;
		repely += dy/(l*l)*weight;
		connections++;
    }

     // Now subtract all forces pulling items together
    double weight = 0.01;
    foreach (GEdge *edge, edgeList) {
        QPointF pos;
		if ( ! edge->destGNode()->isVisible() ) continue;
		if ( ! edge->sourceGNode()->isVisible() ) continue;

		GNode* node = NULL;
        if (edge->sourceGNode() == this ) {
            pos = mapFromItem(edge->destGNode(), 0, 0);
			node = edge->destGNode();
		} else if ( edge->destGNode() == this ) {
            pos = mapFromItem(edge->sourceGNode(), 0, 0);
			node = edge->sourceGNode();
		} else {
			continue;
		}
		if (! node) continue;
	
		QLineF line(pos, QPointF(0, 0));
		qreal dx = line.dx();
        qreal dy = line.dy();
        double l = sqrt(dx * dx + dy * dy);

		float overlapX = (this->boundingRect().width() + node->boundingRect().width())/2.0 - l;
		float overlapY = (this->boundingRect().height() + node->boundingRect().height())/2.0 - l;
		float overlap = std::max( overlapX, overlapY);
		if ( overlap > 1 ) continue;

		foreach (GEdge* e, node->edges() ) if ( e->isVisible() ) weight *= 1.1;
		//cerr << "LINE=" << l << endl;
		//cerr << "pos=" << pos.x() << " pos=" << pos.y() << " " << weight << endl;
		attractx += pos.x() * weight;
		attracty += pos.y() * weight;
    }

	//qDebug() << getId() << " Rp=" << repelx << " " << repely << "  At=" << attractx << " " << attracty;

	qreal xpos = pos().x()+ attractx + repelx;
	qreal ypos = pos().y()+ attracty + repely;
	if ( xpos < 0 || xpos > scene()->width() )  xpos = pos().x();
	if ( ypos < 0 || ypos > scene()->height() ) ypos = pos().y();

    newPos =QPointF(xpos,ypos);
}

bool GNode::advance()
{
    if (newPos == pos()) return false;
    setPos(newPos); return true;
}

void GNode::adjustNeighbors() {
	qDebug() << "GNode::adjustNeighbors()" << getNote();
    setFixedPosition(true);
    foreach(GEdge* e, edgeList ) {
        GNode* node = e->sourceGNode();
        if ( node == this) node = e->destGNode();
        node->calculateForces();
        node->advance();
    }
    setFixedPosition(false);
	this->calculateForces();
	this->advance();
}

double GNode::computeGNodeSize(float) { 
    return 10;
}


QRectF GNode::boundingRect() const
{
    return QRectF(-_boxWidth/2, -_boxHeight/2, _boxWidth, _boxHeight);
}

void GNode::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget *)
{
	int nodesize = computeGNodeSize(10);
	_boxWidth=nodesize;
	_boxHeight=nodesize;
    painter->setBrush(QBrush(Qt::gray));
    painter->drawEllipse(-nodesize/2+1,-nodesize/2+1,nodesize,nodesize);
    painter->setBrush(QBrush(_brush));
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

