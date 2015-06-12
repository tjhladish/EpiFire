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
    _animationTime=0;
    _timerID=0;
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

GNode* GraphWidget::addGNode(int id, void* data) {
   qDebug() << "addGNode() " << id;
    if ( id >= nodelist.size() ) {
			GNode* n = new GNode(0,scene());
            n->setId(id);
			n->setDataReference(data);
			n->setGraphWidget(this);
            nodelist.push_back(n);
			return n;
	} else {
            return nodelist[id];
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

void GraphWidget::removeGNode(GNode*) {}

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
    forceLayout(1);
	resetZoom();
}

float len(float b1, float b2) { return sqrt(b1*b1 + b2*b2); }
float attraction (float d, float k) { return d*d/k; }
float repulsion (float d, float k)  { return k*k/d; }
float cool (float temp, float initial_temp, int rep_max) { 
    temp -= initial_temp / rep_max;
    return temp < 0 ? 0 : temp;
}

void GraphWidget::forceLayout(int iterations=1) {
    //qDebug() << "force layout qdebug";
    //cerr << "force layout cerr\n";
    ForceLayout layout;

    vector<Particle*> particles;

    for( GNode* n: nodelist ) {
   //     cerr << "g" << n->getId() << " " << n->edges().size() << endl;
        Particle* a = new Particle(n->pos().x(), n->pos().y());
        particles.push_back(a);
    }

    for (unsigned int aId = 0; aId < nodelist.size(); ++aId) {
        GNode* n = nodelist[aId];
        Particle* p = particles[aId];
        for (GEdge* e: n->edgesIn()) {
            int bId = e->sourceGNode()->getId();
            p->linksIn.push_back(new Link(particles[bId], p));
       }

       for (GEdge* e: n->edgesOut()) {
            int bId = e->destGNode()->getId();
            p->linksOut.push_back(new Link(p, particles[bId]));
       }

    }

    //for (unsigned int i = 0; i< particles.size(); ++i) {
    //    cerr << "p" << i << " " << (particles[i]->linksIn.size() + particles[i]->linksOut.size()) << endl;
    //}

    layout.doLayout(particles, iterations);

    for(unsigned int i=0; i<nodelist.size(); i++ ) {
        nodelist[i]->setPos( particles[i]->x , particles[i]->y );
      //  qDebug() << particles[i]->x << " " << particles[i]->y;
    }
    //resetZoom();
    invalidateScene();
    for (unsigned int i = 0; i < particles.size(); ++i) delete particles[i];
}

void GraphWidget::randomLayout() { 

    int W=300; int H=300;
    int sceneW = W*1.2; int sceneH = H*1.2;
    scene()->setSceneRect(-sceneW/2,-sceneH/2,sceneW,sceneH);

    fitInView(sceneRect(),Qt::KeepAspectRatio);
    foreach(GNode* n1, nodelist ) {
        float x = (((float) rand())/RAND_MAX*W - W/2)/(n1->edges().size() + 1);
        float y = (((float) rand())/RAND_MAX*H - H/2)/(n1->edges().size() + 1);
        n1->setPos(x,y);
    }
/*
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
    }*/
    //fitInView(sceneRect(),Qt::KeepAspectRatio);
}

void GraphWidget::updateLayout() {}

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


void GraphWidget::layoutOGDF() {}

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

GNode* GraphWidget::locateGNode(int id) {
    if ( id < nodelist.size() ) return nodelist[id];
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
                        forceLayout();
                        break;
                case Qt::Key_Return:
                        forceLayout();
                        break;
                case Qt::Key_N:
						newLayout();
						break;
				default:
						QGraphicsView::keyPressEvent(event);
		}

	scene()->update();
}

//void delay( int millisecondsToWait ) {
//    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
//    while( QTime::currentTime() < dieTime ) {
//        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
//    }
//}

void GraphWidget::timerEvent(QTimerEvent*) {
    //qDebug() << "Timer ID:" << event->timerId();
    QRgb colors[4] = { qRgb(0, 0, 200), qRgb(254, 0, 0), qRgb(254, 254, 0), qRgb(254,254,254) };
    QColor col = Qt::gray;

    //for (unsigned int t = 0; t < node_states.size(); ++t) {
    if (nodeStates.size() > _animationTime) {
        for (unsigned int id = 0; id < nodeStates[_animationTime].size(); ++id) {
            int state = nodeStates[_animationTime][id] == -1 ? 2 :
                        nodeStates[_animationTime][id] ==  0 ? 0 : 1;
            col = colors[state];
            GNode* node = locateGNode(id);
            if (node) node->setBrush(col);
        }
        invalidateScene();

        //delay(500);
        if (++_animationTime >= nodeStates.size()) _animationTime = 0;
    }
}

void GraphWidget::animateNetwork() {
    if (_timerID) killTimer(_timerID);
    _timerID = startTimer(500);
}
