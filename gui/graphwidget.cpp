#include "graphwidget.h" 
#include <iostream>
#include <QDebug>
#include <QFileDialog>

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
    _epiTimerID=0;
    _layoutTimerID=0;
    _animationCounter=0;
    _zoomFactor=1.2;

    savePlotAction = new QAction("Export plot as PNG", this);
    connect( savePlotAction, SIGNAL(triggered()), this, SLOT(savePlot()) );
    saveDataAction = new QAction("Export edgelist (degree > 0 nodes) and node coordinates (all nodes) as CSV files", this);
    connect( saveDataAction, SIGNAL(triggered()), this, SLOT(saveData()) );
}

GraphWidget::~GraphWidget() { 
    clear();
}


void GraphWidget::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    menu.addAction(savePlotAction);
    menu.addAction(saveDataAction);
    menu.exec(event->globalPos());
}

void MyScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
    down =  mouseEvent->buttonDownScenePos(Qt::LeftButton);
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void MyScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
    up =  mouseEvent->scenePos();
    int width =  up.x()-down.x();
    int height = up.y()-down.y();
    //qDebug() << down << " " << up;
    //cerr <<  "selectedArea : " << width << " " << height << endl;
    QRectF area(down.x(),down.y(),width,height);

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    if ( mouseEvent->modifiers() == Qt::AltModifier ) {
        emit(zoomArea(area));
        return;
    }
    if ( mouseEvent->modifiers() == Qt::ControlModifier ) {
        QPainterPath pp;
        pp.addRect(area);
        setSelectionArea(pp);
    }
    return;
};

void GraphWidget::clear() {
    nodelist.clear();
    scene()->clear();
}

GNode* GraphWidget::addGNode(int id){//, void* data) {
//    qDebug() << "addGNode() " << id;
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
        if (e->source() == n1 && e->dest() == n2 ) return e;
        if (e->source() == n2 && e->dest() == n1 ) return e;
    }

    foreach( GEdge* e, n2->edges() ) {
        if (e->source() == n1 && e->dest() == n2 ) return e;
        if (e->source() == n2 && e->dest() == n1 ) return e;
    }
    return NULL;
}

GEdge* GraphWidget::addGEdge(GNode* n1, GNode* n2, string note, void* data) {
    if ( ! n1 || ! n2 ) return NULL;
    GEdge* e = new GEdge();
    e->setSource(n1);
    e->setDest(n2);
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
    if(_epiTimerID) {
        killTimer(_epiTimerID);
        _epiTimerID = 0;
    }
    if(_layoutTimerID) {
        killTimer(_layoutTimerID);
        _layoutTimerID = 0;
    }
}

void GraphWidget::forceLayout(int iterations=1) {
    ForceLayout layout;

    QRectF r = scene()->sceneRect();
    double s = 0.95; // shrink plot region used
    layout.set_dimensions(s*r.left(), s*r.right(), s*r.top(), s*r.bottom());
    layout.doLayout(nodelist, iterations);

    invalidateScene();
}

void GraphWidget::randomLayout() { 
    QRectF r = scene()->sceneRect();
    int W = 0.75*r.width();
    int H = 0.75*r.height();

    foreach(GNode* n1, nodelist ) {
        float x = (((float) rand())/RAND_MAX*W - W/2)/(n1->edges().size() + 1);
        float y = (((float) rand())/RAND_MAX*H - H/2)/(n1->edges().size() + 1);
        n1->setPos(x,y);
        n1->initializePreviousPosition();
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
            foreach(GEdge* e, n->edges()) { qDebug() << "\t\t:" << e->dest()->getId() << " " << e->source()->getId(); }
            foreach(GEdge* e, n->edgesOut()) { qDebug() << "\t\tout:" << e->dest()->getId(); }
            foreach(GEdge* e, n->edgesIn())  { qDebug() << "\t\tin:" <<  e->source()->getId(); }
        }
    }
}

GNode* GraphWidget::locateGNode(int id) {
    if ( id < nodelist.size() ) return nodelist[id];
    return NULL;
}

void GraphWidget::keyPressEvent(QKeyEvent *event){
    switch (event->key()) {
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

void GraphWidget::wheelEvent(QWheelEvent *event){
    if (event->delta() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
}


void GraphWidget::timerEvent(QTimerEvent* event) {
    if (event->timerId() == _epiTimerID) {
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
    } else {
        forceLayout(1);
        if (++_animationCounter > 500) {
            killTimer(_layoutTimerID);
            _layoutTimerID = 0;
            _animationCounter = 0;
        }
    }
}


void GraphWidget::animateNetwork() {
    if (_epiTimerID) killTimer(_epiTimerID);
    _epiTimerID = startTimer(500);
}


void GraphWidget::relaxNetwork() {
    if (_layoutTimerID) return; // already running
    _layoutTimerID = startTimer(1);
}


void GraphWidget::resizeEvent(QResizeEvent *){
    QPointF center = mapToScene(viewport()->rect().center());
    int sceneW = 0.9*width();
    int sceneH = 0.9*height();
    scene()->setSceneRect(-sceneW/2,-sceneH/2,sceneW,sceneH);
    centerOn(center);
}

void GraphWidget::saveData() {
    QString startdir = ".";
    QString netFilename = QFileDialog::getSaveFileName(this, "Select file to save to:", startdir, "CSV Files(*.csv)");
    QString layoutFilename(netFilename);
    layoutFilename.replace(QString(".csv"), QString("_layout.csv"));

    QFile fileNet(netFilename);
    QFile fileLayout(layoutFilename);
    if (!fileNet.open(QIODevice::WriteOnly | QIODevice::Text)
     or !fileLayout.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream outN(&fileNet);
    QTextStream outL(&fileLayout);

    for (GNode* node: nodelist) {
        // output location of node to one file
        outL << node->getId() << "," << node->x() << "," << node->y() << endl;
        // output edgelist to the other file
        // one node if node has no edges
        //if (node->edgesOut().size() == 0) {
        //    outN << node->getId() << endl;
        //    continue;
        //}
        for (GEdge* edge: node->edgesOut()) {
            GNode* other = edge->dest();
            outN << node->getId() << "," << other->getId() << endl;
        }
    }
    fileNet.close();
    fileLayout.close();
}


void GraphWidget::savePlot() {
    QString startdir = ".";
    QString filename = QFileDialog::getSaveFileName(
        this, "Select file to save to", startdir, "PNG Image Files(*.png)");

    QPixmap image(width(),height());
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    render(&painter);

    image.save(filename,"PNG");
}
