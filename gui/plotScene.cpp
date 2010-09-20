#include "plotScene.h"

PlotScene::PlotScene(QObject* parent):QGraphicsScene(parent) {
    topM=bottomM=leftM=rightM=20;
    topA=bottomA=leftA=rightA=20;

    initialize();
    reDefinePlotRegions();
}

void PlotScene::initialize() { 
    dataArea = new PlotRegion(0,0,this);

    leftMargin   = new PlotRegion(0,0,this );
    rightMargin  = new PlotRegion(0,0,this );
    topMargin    = new PlotRegion(0,0,this );
    bottomMargin = new PlotRegion(0,0,this );


    leftAxis   = new PlotRegion( 0,0,   this );
    rightAxis  = new PlotRegion( 0,0,   this );
    topAxis    = new PlotRegion( 0,0,   this );
    bottomAxis = new PlotRegion( 0,0,   this );

}

void PlotScene::reDefinePlotRegions() { 
    int dw = width() - leftA - rightA - leftM - rightM;
    int dh = height() - topA - bottomA - topM - bottomM;
    int topBorderH = topM + topA;
    int leftBorderW = leftM + leftA;

    dataArea->setWidthHeight(dw,dh);

    leftMargin->setWidthHeight(leftM,   dh);
    rightMargin->setWidthHeight( rightM,  dh);
    topMargin->setWidthHeight(dw,      topM);
    bottomMargin->setWidthHeight( dw,      bottomM);


    leftAxis->setWidthHeight( leftA,     dh);
    rightAxis->setWidthHeight( rightA,    dh);
    topAxis->setWidthHeight( dw,        topA);
    bottomAxis->setWidthHeight( dw,        bottomA);

    dataArea->setPos(leftBorderW, topBorderH);
    topMargin->setPos(leftBorderW, 0);
    bottomMargin->setPos(leftBorderW, topBorderH + dh + bottomAxis->height());
    leftMargin->setPos(0, topBorderH );
    rightMargin->setPos(leftBorderW + dw + rightAxis->width(), topBorderH);

    topAxis->setPos(leftBorderW, topMargin->height());
    bottomAxis->setPos(leftBorderW, topBorderH + dh);
    leftAxis->setPos(leftMargin->width(), topBorderH );
    rightAxis->setPos(leftBorderW + dw, topBorderH );

    leftMargin->setBrush(QBrush(Qt::red));
    rightMargin->setBrush(QBrush(Qt::blue));
    topMargin->setBrush(QBrush(Qt::green));
    bottomMargin->setBrush(QBrush(Qt::yellow));
}
 

