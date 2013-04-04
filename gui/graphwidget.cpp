#include "graphwidget.h" 
#include <iostream>
#include <QDebug>

void MyScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
	down =  mouseEvent->buttonDownScenePos(Qt::LeftButton);
	QGraphicsScene::mousePressEvent(mouseEvent);
}


void MyScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
		up =  mouseEvent->scenePos();
		int width =  up.x()-down.x();
		int height = up.y()-down.y();
		qDebug() << down << " " << up;
		//cerr <<  "selectedArea : " << width << " " << height << endl;
		QRectF area(down.x(),down.y(),width,height);

		QGraphicsScene::mouseReleaseEvent(mouseEvent);
		if ( mouseEvent->modifiers() == Qt::ControlModifier ) {
			emit(zoomArea(area));
		}  
		return;
};


GraphWidget::GraphWidget()
{
    setScene(new MyScene(this));
    //setScene(new QGraphicsScene(this));
	scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene()->setSceneRect(-width()/2,-height()/2,width(),height()); //scene = physical dimentions

    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
	setDragMode(QGraphicsView::RubberBandDrag);
    setGNodeSizeNormalization(GraphWidget::AbsoluteSize);

	setGNodeSizeScale(1);
	setLabelSizeScale(1);
	setGEdgeSizeScale(1);
	setLayoutAlgorithm(FMMM);

	connect(scene(),SIGNAL(zoomArea(QRectF)), this,SLOT(zoomArea(QRectF)));
	layoutTree = new QTreeWidget(this);
	layoutTree->hide();

    _title = NULL;
	_averageGEdgeSize=100;

}

GraphWidget::~GraphWidget() { 
	clear();
}

void GraphWidget::clear() { 
    layoutMap.clear();
	nodelist.clear();
	scene()->clear();
	layoutTree->clear();
}

GNode* GraphWidget::addGNode(string id, void* data) {
   qDebug() << "addGNode() " << id.c_str();
	if ( nodelist.count(id.c_str()) == 0 ) {
			GNode* n = new GNode(0,scene());
			n->setId(id.c_str());
			n->setNote(id.c_str());
			n->setDataReference(data);
			n->setGraphWidget(this);
			nodelist[id.c_str()]=n;
			return n;
	} else {
			return nodelist[id.c_str()];
	}
}

GEdge* GraphWidget::findGEdge(GNode* n1, GNode* n2) {
		if (!n1 || !n2 ) return NULL;

		foreach( GEdge* e, n1->edges() ) {
			if (e->sourceGNode() == n1 && e->destGNode() == n2 ) return e;
			if (e->sourceGNode() == n2 && e->destGNode() == n1 ) return e;
		}

		foreach( GEdge* e, n2->edges() ) {
			if (e->sourceGNode() == n1 && e->destGNode() == n2 ) return e;
			if (e->sourceGNode() == n2 && e->destGNode() == n1 ) return e;
		}
		return NULL;
}

GEdge* GraphWidget::addGEdge(GNode* n1, GNode* n2, string note, void* data) {
		if ( ! n1 || ! n2 ) return NULL; 
		GEdge* e = new GEdge();
		e->setSourceGNode(n1);
		e->setDestGNode(n2);
		e->setNote(note.c_str());
		e->setData(data);
		n1->addGEdge(e);
		n2->addGEdge(e);
		scene()->addItem(e);
		return(e);
}

void GraphWidget::removeGNode(GNode* n) {
	if (!n) return;

	qDebug() << "Removing: " << n->getId() << endl;
	n->setVisible(false);
	scene()->removeItem(n);
	foreach (GEdge* e, n->edges() ) {
		e->setVisible(false);
		scene()->removeItem(e);
		n->removeGEdge(e);
		if (e->sourceGNode() == n) e->destGNode()->removeGEdge(e);
		if (e->destGNode()   == n) e->sourceGNode()->removeGEdge(e);
		delete(e);
	}

	if (layoutMap.contains(n)) {
		layoutMap.clear();
		layoutTree->clear();
	}

	if (nodelist.contains(n->getId()) ) {
		nodelist.remove(n->getId());
	}

	delete(n);
}

void GraphWidget::removeSelectedGNodes() {
    foreach (QGraphicsItem *item, scene()->selectedItems()) {
		if (GNode *node = qgraphicsitem_cast<GNode *>(item)) {
			removeGNode(node);
		}
	}
}

QList<GNode*> GraphWidget::getGNodes(int type) {
	QList<GNode*>x;
	foreach(GNode* n, nodelist ) if (n->molClass() == type) x<<n; 
	return x;
}

void GraphWidget::itemMoved()
{
	cerr << "Item Moved" << endl;
}

void GraphWidget::randomGNodePositions() { 
    foreach (QGraphicsItem *item, scene()->items()) {
          if (qgraphicsitem_cast<GNode *>(item)) item->setPos(-100 + qrand() % 100, -100 + qrand() % 100);
    }
}


void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    return;
    Q_UNUSED(rect);
	float w = scene()->width();
	float h = scene()->height();
    painter->setPen(Qt::lightGray);
    painter->setPen(Qt::lightGray);
	painter->drawLine(QPointF(0,0), QPointF(w/2, 0));
	painter->drawLine(QPointF(0,0), QPointF(0, h/2));
	painter->drawEllipse(QPointF(0,0), 5,5);
	painter->drawText(w/2-20,h/2-20,QString::number(w) + " " + QString::number(h) );
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::setTitle(const QString& titleText) { 
    QFont f("Helvetica");
	float ratio = 1; 
	if ( height() )  ratio = scene()->height()/height();
	int fontSize= height()*0.03*ratio;
	f.setPixelSize(fontSize);
    if (!_title) { 
		_title = scene()->addText(titleText, f);
		_title->setFlag(QGraphicsItem::ItemIsMovable);
	}
	if (_title )  {
		_title->setHtml(titleText);
		_title->setPos(-scene()->width()/2, -scene()->height()/2);
	}
	
}

void GraphWidget::updateSceneRect() {
	float minX=0;
	float minY=0;
	float maxX=0;
	float maxY=0;
	int itemCount=0;
	float centerX = 0;
	float centerY = 0;

	foreach (GNode* item, nodelist ) {
			if (!item->isVisible()) continue;
			if (item->pos().x() < minX) minX = item->pos().x();
			if (item->pos().x() > maxX) maxX = item->pos().x();
			if (item->pos().y() < minY) minY = item->pos().y();
			if (item->pos().y() > maxY) maxY = item->pos().y();
			centerX += item->pos().x();
			centerY += item->pos().y();
			itemCount++;
	}

	if (itemCount) { centerX /= itemCount; centerY /= itemCount; }
	QPointF center(centerX,centerY);

	//shift nodes to center
	//foreach (QGraphicsItem *item, scene()->items()) item->setPos(item->pos()+center);
	//minX += centerX; maxX += centerX;
	//minY += centerY; maxY += centerY;
	minX -= 10; maxX += 10; minY -= 10; maxY += 10;
	float W = maxX-minX;
	float H = maxY-minY;
	
	scene()->setSceneRect(-W/2-50,-H/2-50,W+100,H+100);
    fitInView(sceneRect(),Qt::KeepAspectRatio);
    qDebug() << "updateSceneRect:  WxH=" << W << " " << H << " " << center << endl;
	return;


	if (W < 300 ) W=300;
	if (H < 300 ) H=300;

	float aH = H*0.2;
	float aW = W*0.2;
	if ( scene()->width() > W )  aW=0;
	if ( scene()->height() > H ) aH=0;
    scene()->setSceneRect(-W/2-aW/2,-H/2-aH/2,W+aW,H+aH);
    fitInView(sceneRect(),Qt::KeepAspectRatio);
    qDebug() << "updateSceneRect:  WxH=" << W << " " << H << " " << center << endl;
}

	

void GraphWidget::resetZoom() { 
    cerr << "resetZoom()" << endl;
	computeAvgGEdgeLength();

    float minX=0;
    float minY=0;
    float maxX=0;
    float maxY=0;
	int itemCount=0;
	float centerX = 0;
	float centerY = 0;

    foreach (QGraphicsItem *item, scene()->items()) {
			if ( item->isVisible() ) {
					if (qgraphicsitem_cast<GNode *>(item)) {
							if (item->pos().x() < minX) minX = item->pos().x();
							if (item->pos().x() > maxX) maxX = item->pos().x();
							if (item->pos().y() < minY) minY = item->pos().y();
							if (item->pos().y() > maxY) maxY = item->pos().y();
							centerX += item->pos().x();
							centerY += item->pos().y();
							itemCount++;
					}
			}
    }

	minX -= 10; maxX += 10; minY -= 10; maxY += 0;
	if ( itemCount == 0 ) return;

	centerX /= itemCount;
	centerY /= itemCount;

	float scale=1;
	float W = maxX-minX;
	float H = maxY-minY;
    cerr << "GraphWidget::resetZoom() xDim=" << W << " " << H << " " << scale << endl;

    if ( _averageGEdgeSize > 0 ) { scale = 100/_averageGEdgeSize; } 
 	W*=scale; 
	H*=scale;
	if (W<300) W=300;
	if (H<300) H=300;
    
    foreach (QGraphicsItem *item, scene()->items()) {
        if ( item->isVisible() ) {
            if (qgraphicsitem_cast<GNode *>(item)) {
                float x = item->pos().x();
                float y = item->pos().y();
                float newx =  (x-centerX)*scale;
                float newy =  (y-centerY)*scale;
                item->setPos(newx, newy);

            } 
        }
    }

	float aH = H*0.1;
	float aW = W*0.1;
    scene()->setSceneRect(-W/2-aW/2,-H/2-aH/2,W+aW,H+aH);
    fitInView(sceneRect(),Qt::KeepAspectRatio);
	scene()->update();
	computeAvgGEdgeLength();
    cerr << "GraphWidget::resetZoom() sceneSize WxH=" << W << " " << H << " " << scale << endl;
}

void GraphWidget::newLayout() {
	cerr << "newLayout() " << endl;
	clearLayout();
//	layoutOGDF();
    randomLayout();
	resetZoom();
}

float len(float b1, float b2) { return sqrt(b1*b1 + b2*b2); }
float attraction (float d, float k) { return d*d/k; }
float repulsion (float d, float k)  { return k*k/d; }
float cool (float temp, float initial_temp, int rep_max) { 
    temp -= initial_temp / rep_max;
    return temp < 0 ? 0 : temp;
}

void GraphWidget::randomLayout() { 

    int W=300; int H=300;
    int sceneW = W*1.2; int sceneH = H*1.2;
    int A = W*H;
    float k = sqrt((float) A/nodelist.size());
    scene()->setSceneRect(-sceneW/2,-sceneH/2,sceneW,sceneH);

    fitInView(sceneRect(),Qt::KeepAspectRatio);
    foreach(GNode* n1, nodelist ) {
        float x = (((float) rand())/RAND_MAX*W - W/2)/(n1->edges().size() + 1);
        float y = (((float) rand())/RAND_MAX*H - H/2)/(n1->edges().size() + 1);
        n1->setPos(x,y);
    }

    float t_init = 100;
    float t = t_init;
    int rep_max = 50;

    map<GNode*, vector<float> > newDisp;

    for(int itr=0; itr<rep_max; itr++ ) {
        
        foreach(GNode* n1, nodelist ) {
            // calculate repulsion
            vector<float> disp(2,0);
            newDisp[n1] = disp;
            float rep = 0.0;
            foreach(GNode* n2, nodelist ) {
                if (n1 == n2) continue;
                float delta_x = n1->pos().x() - n2->pos().x();
                float delta_y = n1->pos().y() - n2->pos().y();
                float dist = len(delta_x, delta_y);
//                dist = dist == 0 ? 0.00001 : dist;
                rep = repulsion(dist, k);
                newDisp[n1][0] += (delta_x/dist)*rep;
                newDisp[n1][1] += (delta_y/dist)*rep;
            }
            
            // calculate attration
            //float aX=0; float aY=0;
            float att = 0.0;
            foreach (GEdge* e, n1->edges() ) { 
                GNode* n2 = e->destGNode(); if(n2 == n1) n2 = e->sourceGNode();
                if (newDisp.count(n2) == 0) newDisp[n2] = disp; 
                float delta_x = n1->pos().x() - n2->pos().x();
                float delta_y = n1->pos().y() - n2->pos().y();
                float dist = len(delta_x, delta_y);
//                dist = dist == 0 ? 0.00001 : dist;
                att = attraction(dist, k);
                newDisp[n1][0] -= (delta_x/dist)*att;
                newDisp[n1][1] -= (delta_y/dist)*att;
                newDisp[n2][0] += (delta_x/dist)*att;
                newDisp[n2][1] += (delta_y/dist)*att;
            }

        }
        foreach(GNode* n1, nodelist ) {
            float disp_x = newDisp[n1][0];
            float disp_y = newDisp[n1][1];

            float disp_magnitude = len(disp_x, disp_y);
//            disp_magnitude = disp_magnitude == 0 ? 0.00001 : disp_magnitude;

            float newX = n1->pos().x() + (disp_x/disp_magnitude)*min(disp_magnitude,t);
            newX = min((float) W/2, max((float) -W/2, newX));

            float newY = n1->pos().y() + (disp_y/disp_magnitude)*min(disp_magnitude,t);
            newY = min((float) H/2, max((float) -H/2, newY));

            n1->setPos( newX, newY);

            t = cool(t, t_init, rep_max);
        }
    }
    //fitInView(sceneRect(),Qt::KeepAspectRatio);
}

void GraphWidget::updateLayout() {
	cerr << "updateLayout() " << endl;
	if (nodelist.size() == 0 ) return;
	setLayoutAlgorithm(Balloon);

    QPointF zero(0,0);
    foreach(GNode* x, nodelist) if(x && x->pos() != zero) x->setFixedPosition(true); 
    foreach(GNode* x, nodelist) if(x && !layoutMap.contains(x)) recursiveDepth(x,0); 

	if (layoutMap.size() > 0) {
		for(int i=0; i<layoutTree->topLevelItemCount() ; i++ ) {
			QTreeWidgetItem* x = layoutTree->topLevelItem(i);
			if (x) recursiveDraw(x);
		}
	} else { 
		layoutOGDF();
	}

    foreach(GNode* x, nodelist){ if(x) x->setFixedPosition(false); }
	//updateSceneRect();
}

void GraphWidget::adjustLayout() {
	cerr << "adjustLayout() " << endl;
	computeAvgGEdgeLength();

	foreach (GNode* n, nodelist ) {
		foreach (GEdge* e, n->edges() ) {
			if ( e->length() > 2*getAvgGEdgeLength() ) e->hide();
		}
	}
}

void GraphWidget::adjustLayout(GNode* n1) {
	cerr << "adjustLayout(n1) " << endl;
		foreach (GNode* n2, nodelist) {
				if (n1 != n2 && n2->isVisible() && n1->isVisible() && n1->collidesWithItem(n2,Qt::IntersectsItemBoundingRect)) {
						n2->calculateForces();
						n2->advance();
				}
		}
}


void GraphWidget::clearLayout() {
	cerr << "clearLayout() " << endl;
	layoutTree->clear();
    layoutMap.clear();
	foreach(GNode* n, nodelist) { n->setDepth(-1); n->setPos(0,0); }
}

void GraphWidget::addToTree(GNode* a, GNode* b) {
	//qDebug() << "addToTree: " << a->getNote() << " " << b->getNote();

    if(!a) { //missing parent
       foreach(GNode* n, nodelist) {
           if(layoutMap.contains(n) ){ 
               QList<GEdge*> edges = n->findConnectedGEdges(b); 
               foreach(GEdge* e, edges) { 
                   if (layoutMap.contains(e->sourceGNode())) { a = e->sourceGNode(); break; }
                   if (layoutMap.contains(e->destGNode()))   { a = e->destGNode(); break; }
               }
           }
       }
    }

    if (!a || !b ) return;
	if (layoutMap.contains(a) == false) return;
	if (layoutMap.contains(b) == true) return;
	QTreeWidgetItem* parent = layoutMap[a];
	if (!parent) return;

    QTreeWidgetItem* item  = new QTreeWidgetItem(parent);
    item->setText(0,b->getId());
    layoutMap[b]=item;
}

void GraphWidget::recursiveDepth(GNode* n0,int depth) {
	//cerr << "recursiveDepth() " << endl;

	QTreeWidgetItem* parent=0;
	if (layoutMap.contains(n0)) { 
        parent=layoutMap[n0];
    } else {
        foreach(GEdge* e, n0->edges() ) {
            GNode* other = e->sourceGNode();
            if (other == n0 ) other= e->destGNode();
            if (other == n0 ) continue;
	        if (layoutMap.contains(other)) { parent=layoutMap[other]; n0=other; break; }
        }
    } 

	if (!parent) {
		QTreeWidgetItem* item  = new QTreeWidgetItem(layoutTree);
		item->setText(0,n0->getId());
		if (layoutMap.size()) n0->setPos( (float) scene()->width(), 0 );
		layoutMap[n0]=item;
	    parent = item;
	}
	if (!parent) return;

	for(int i=0; i<depth+1; i++) cerr << " ";
	qDebug() << depth << " " << n0->getId() << " " << n0->getDepth() << " " << n0->boundingRect().width();
	QList<GNode*>newnodes;
    foreach(GEdge* e, n0->edgesOut() ) {
        GNode* other= e->destGNode();
        if (!other || other == n0 || layoutMap.contains(other))  continue;
        addToTree(n0,other); 
        newnodes.push_back(other); 
    }

    foreach(GEdge* e, n0->edgesIn() ) {
        GNode* other = e->sourceGNode();
        if (!other || other == n0 || layoutMap.contains(other))  continue;
        addToTree(n0,other); 
        newnodes.push_back(other); 
    }

	foreach(GNode* n, newnodes) { recursiveDepth(n,depth+1); }

	
}



void GraphWidget::computeAvgGEdgeLength() {
	int count=0;// double len=0;
    _averageGEdgeSize=50; 

	vector<float>lvector;
	foreach(GNode* n, nodelist) { 
		foreach (GEdge* e, n->edges() ) {
           if (e->sourceGNode()->isVisible() && e->destGNode()->isVisible() ){
               QPointF p1 = e->sourceGNode()->pos();
               QPointF p2 = e->destGNode()->pos();
               float l = QLineF(p1,p2).length();
               if (l) { lvector.push_back(l); }
               count++; 
            }
		}
	}
    if (lvector.size() > 0) _averageGEdgeSize = 100;
	cerr << "AvgGEdge=" << _averageGEdgeSize << endl;

    return;
}


void GraphWidget::deepChildCount(QTreeWidgetItem* x, int* count) {
	if (x->childCount()==0) return;
	int childcount=x->childCount();
	(*count) += childcount;
	for(int i=0; i<childcount;i++) deepChildCount(x->child(i),count);
	qDebug() << "deepChildCount: " << x->text(0) << " " << *count;
}

void GraphWidget::recursiveDraw(QTreeWidgetItem* parent) {
	if (!parent) return;
	//int deepcount=0; deepChildCount(item,&deepcount);
	int childcount=parent->childCount();
	QString name = parent->text(0);
	GNode* parentGNode =  locateGNode(name);
	if (parentGNode == NULL ) return;

	float px = parentGNode->pos().x();
	float py = parentGNode->pos().y();

	parentGNode->setNewPos(px,py);

    int half = childcount/2;
	if (childcount== 1) half=0;

	float angle0=atan2(py,px);

	for(int i=0; i<childcount;i++) {
		QTreeWidgetItem* child = parent->child(i);
		//QVariant v =  child->data(0,Qt::UserRole);
		//GNode* n =  (GNode*) v.value<QGraphicsItem*>();
		GNode* n = locateGNode(child->text(0));
		if (n) {
           float dist=_averageGEdgeSize;
		   if (dist < 10 ) dist=10;
		   if (childcount > 2) dist *= log2(childcount);

		   if (getLayoutAlgorithm() == FMMM ) {
				   float cy= py+(i-half)*dist; 
				   float cx= px+dist;
				   n->setNewPos(cx,cy);
		   } else if (getLayoutAlgorithm() == Circular ) {
				   float angle = (float) (i+1)/childcount*2*Pi;
				   float cx= px+cos(angle0+angle)*dist; 
				   float cy= py+sin(angle0+angle)*dist;
				   n->setNewPos(cx,cy);
		   } else if (getLayoutAlgorithm() == Balloon) {
				   float angle = angle0+(float) (i-half)/childcount*0.9*Pi;
				   float cx= px+cos(angle)*dist; 
				   float cy= py+sin(angle)*dist;
				   n->setNewPos(cx,cy);
		   }
		}
	}

	for(int i=0; i<childcount;i++) { recursiveDraw(parent->child(i)); }
}


void GraphWidget::layoutOGDF() { 
    cerr << "layoutOGDF() " << endl;
	if (nodelist.size()==0) return;

	QList<GNode*>metabolites =getGNodes(GNode::Unassigned); 

	foreach(GNode* n, metabolites) { 
		if(layoutMap.count(n)) continue;
		if(n->edgesIn().size()>0 ) continue;
		recursiveDepth(n,0);
	}
	foreach(GNode* n, metabolites) { if(layoutMap.count(n)==0) recursiveDepth(n,0); }

	for(int i=0; i<layoutTree->topLevelItemCount() ; i++ ) {
		QTreeWidgetItem* x = layoutTree->topLevelItem(i);
		recursiveDraw(x);
	}
}

void GraphWidget::dump() {
	foreach(GNode* n, nodelist) { 
		qDebug() << "Network:" << n->getId(); 
		if (n->isVisible()) { 
			qDebug() << "\t GNode:" << n->getId(); 
			qDebug() << "\t GEdge: ";
			foreach(GEdge* e, n->edges()) { qDebug() << "\t\t:" << e->destGNode()->getId() << " " << e->sourceGNode()->getId(); }
			foreach(GEdge* e, n->edgesOut()) { qDebug() << "\t\tout:" << e->destGNode()->getId(); }
			foreach(GEdge* e, n->edgesIn())  { qDebug() << "\t\tin:" <<  e->sourceGNode()->getId(); }
		}
    }
}

GNode* GraphWidget::locateGNode(QString id) {
	if ( nodelist.count(id) ) return nodelist[id];
	return NULL;
}

void GraphWidget::keyPressEvent(QKeyEvent *event)
{
		switch (event->key()) {
				case Qt::Key_Delete:
						removeSelectedGNodes();
						break;
				case Qt::Key_0:
						resetZoom();	
						break;
				case Qt::Key_Minus:
						zoomOut();
						break;
				case Qt::Key_Plus:
						zoomIn();
						break;
				case Qt::Key_Enter:
						adjustLayout();
						break;
				case Qt::Key_N:
						newLayout();
						break;
				default:
						QGraphicsView::keyPressEvent(event);
		}

	scene()->update();
}

