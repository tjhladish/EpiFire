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

GraphWidget::GraphWidget() {
    setScene(new MyScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene()->setSceneRect(-width()/2,-height()/2,width(),height()); //scene = physical dimentions

    setCacheMode(CacheBackground);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
    setDragMode(QGraphicsView::RubberBandDrag);
    setLayoutAlgorithm(FMMM);

    connect(scene(),SIGNAL(zoomArea(QRectF)), this,SLOT(zoomArea(QRectF)));
    _animationTime=0;
    _timerID=0;
}

GraphWidget::~GraphWidget() { 
    clear();
}

void GraphWidget::clear() { 
    nodelist.clear();
    scene()->clear();
}

GNode* GraphWidget::addGNode(int id){//, void* data) {
    qDebug() << "addGNode() " << id;
    if ( id >= nodelist.size() ) {
        GNode* n = new GNode(0,scene());
        n->setId(id);
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
    e->setGraphWidget(this);
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


void GraphWidget::recenterItems() {
    cerr << "recenterItems()" << endl;
    int itemCount=0;
    float centerX = 0;
    float centerY = 0;

    foreach (QGraphicsItem *item, scene()->items()) {
        if ( item->isVisible() ) {
            if (qgraphicsitem_cast<GNode *>(item)) {
                centerX += item->pos().x();
                centerY += item->pos().y();
                itemCount++;
            }
        }
    }

    if ( itemCount == 0 ) return;

    centerX /= itemCount;
    centerY /= itemCount;
    centerOn(centerX, centerY);
}


void GraphWidget::resetZoom() { 
    cerr << "resetZoom()" << endl;
    QRectF bounds = scene()->itemsBoundingRect();
    //qDebug() << "bounds: " << bounds;
    ensureVisible(bounds);
    invalidateScene();
}

void GraphWidget::newLayout() {
    cerr << "newLayout() " << endl;
    clearLayout();
    randomLayout();
    forceLayout(1);
    resetZoom();
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

    for (int aId = 0; aId < nodelist.size(); ++aId) {
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

    QRectF r = scene()->sceneRect();
    double s = 0.95; // shrink plot region used
    layout.set_dimensions(s*r.left(), s*r.right(), s*r.top(), s*r.bottom());
    layout.doLayout(particles, iterations);

    for(int i=0; i<nodelist.size(); i++ ) {
        nodelist[i]->setPos( particles[i]->x , particles[i]->y );
        //  qDebug() << particles[i]->x << " " << particles[i]->y;
    }
    invalidateScene();
    for (unsigned int i = 0; i < particles.size(); ++i) delete particles[i];
}

void GraphWidget::randomLayout() { 
    QRectF r = scene()->sceneRect();
    int W = 0.95*r.width();
    int H = 0.95*r.height();

    fitInView(sceneRect(),Qt::KeepAspectRatio);
    foreach(GNode* n1, nodelist ) {
        float x = (((float) rand())/RAND_MAX*W - W/2)/(n1->edges().size() + 1);
        float y = (((float) rand())/RAND_MAX*H - H/2)/(n1->edges().size() + 1);
        n1->setPos(x,y);
    }
}


void GraphWidget::clearLayout() {
    cerr << "clearLayout() " << endl;
    foreach(GNode* n, nodelist) { n->setPos(0,0); }
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

GNode* GraphWidget::locateGNode(int id) {
    if ( id < nodelist.size() ) return nodelist[id];
    return NULL;
}

void GraphWidget::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
        case Qt::Key_Delete:
            removeSelectedGNodes();
            break;
        case Qt::Key_0:
            resetZoom();
            break;
        case Qt::Key_1:
            fitInView(scene()->sceneRect());
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

void GraphWidget::timerEvent(QTimerEvent*) {
    QRgb colors[4] = { qRgb(0, 0, 200), qRgb(254, 0, 0), qRgb(254, 254, 0), qRgb(254,254,254) };
    QColor col = Qt::gray;

    if (nodeStates.size() > _animationTime) {
        for (unsigned int id = 0; id < nodeStates[_animationTime].size(); ++id) {
            int state = nodeStates[_animationTime][id] == -1 ? 2 :
                        nodeStates[_animationTime][id] ==  0 ? 0 : 1;
            col = colors[state];
            GNode* node = locateGNode(id);
            if (node) node->setBrush(col);
        }
        invalidateScene();

        if (++_animationTime >= nodeStates.size()) _animationTime = 0;
    }
}

void GraphWidget::animateNetwork() {
    if (_timerID) killTimer(_timerID);
    _timerID = startTimer(500);
}

void GraphWidget::resizeEvent(QResizeEvent *){
    int sceneW = 0.9*width();
    int sceneH = 0.9*height();
    scene()->setSceneRect(-sceneW/2,-sceneH/2,sceneW,sceneH);
    resetZoom();
}
