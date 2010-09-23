#include "plotScene.h"

PlotScene::PlotScene(QObject* parent):QGraphicsScene(parent) {
    topM=30;bottomM=25;leftM=25;rightM=35;
    topA=0;bottomA=25;leftA=40;rightA=0;

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

    xlabel      = new PlotText(0,this);
    ylabel      = new PlotText(0,this);
    title       = new PlotText(0,this);

    xAxis = new Axis(0, 0.0, 1.0, 10, false, false);
    yAxis = new Axis(1, 0.0, 1.0, 10, false, false);

    xAxis->setParentItem(bottomAxis);
    yAxis->setParentItem(leftAxis);
    xlabel->setParentItem(bottomMargin);
    ylabel->setParentItem(leftMargin);
    title->setParentItem(topMargin);

    ylabel->rotate(-90);
    
    xAxis->hide();
    yAxis->hide();
}

void PlotScene::clearPlot() {

    clear(); //delete all items from the scene
    initialize();   //generate default items
    reDefinePlotRegions();  //resize boxes
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

    leftAxis->setWidthHeight( leftA,     height());
    rightAxis->setWidthHeight( rightA,   height());
    topAxis->setWidthHeight( width(),        topA);
    bottomAxis->setWidthHeight( width(),        bottomA);

    dataArea->setPos(leftBorderW, topBorderH);
    topMargin->setPos(leftBorderW, 0);
    bottomMargin->setPos(leftBorderW, topBorderH + dh + bottomAxis->height());
    leftMargin->setPos(0, topBorderH );
    rightMargin->setPos(leftBorderW + dw + rightAxis->width(), topBorderH);

    topAxis->setPos(0, topMargin->height());
    bottomAxis->setPos(0, topBorderH + dh);
    leftAxis->setPos(leftMargin->width(), 0 );
    rightAxis->setPos(leftBorderW + dw, 0 );

    int axisOffset = 5; //spacing away from dataArea
    xAxis->setPos(leftBorderW,-bottomA + xAxis->getMajTickLen() + axisOffset);
    yAxis->setPos(leftA - yAxis->getMajTickLen() - axisOffset,topBorderH);

    QFont font = this->font();

    xlabel->setFont(font);
    xlabel->setPlainText(xlabelText);
    ylabel->setFont(font);
    ylabel->setPlainText(ylabelText);

    QFontMetrics fm(font);
    xlabel->setPos( bottomMargin->width()/2 - fm.width(xlabelText)/2, 0);
    ylabel->setPos( 0, leftMargin->height()/2 + fm.width(ylabelText)/2);

    font.setBold(true);
    title->setFont(font);
    title->setPlainText(titleText);
    title->setPos( 20, topMargin->height()/2 - fm.height()/2);
}
 

QVector2D PlotScene::getDataAreaDim() {
    return QVector2D(dataArea->width(), dataArea->height()); 
}
