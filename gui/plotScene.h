#ifndef PLOTSCENE_H
#define PLOTSCENE_H

#include <QtGui>
#include <QGraphicsScene>
#include "plotRegion.h"
 #include <QVector4D>

class PlotRegion;

class PlotScene : public QGraphicsScene
{
    Q_OBJECT

    friend class PlotRegion;
    friend class PlotView;

    public:

        PlotScene(QObject* parent);
        void initialize();
        void reDefinePlotRegions();
        void setXrange(float a, float b) { minX=a; maxX=b; }
        void setYrange(float a, float b) { minY=a; maxY=b; }

        float getMinX() { return minX; }
        float getMinY() { return minY; }
        float getMaxX() { return maxX; }
        float getMaxY() { return maxY; }
 //       float toPlotX( float x ) { return dataArea->toPlotX(x); }
  //      float toPlotY( float y ) { return dataArea->toPlotY(y); }

        void setLabel(QString l, qreal x, qreal y) { label=addText(l); label->setPos(x,y); }
        QGraphicsTextItem* getLabel() {return label;}
        void setMarginsDim(int top, int bottom, int left, int right) { topM=top; bottomM=bottom; leftM=left; rightM=right; reDefinePlotRegions(); }
        void setAxesDim(int top, int bottom, int left, int right) { topA=top; bottomA=bottom; leftA=left; rightA=right; reDefinePlotRegions(); }
        QVector4D getMarginDim() { return QVector4D(topM,bottomM,leftM,rightM); }
        QVector4D getAxesDim() { return QVector4D(topM,bottomM,leftM,rightM); }

   private:
        PlotRegion* dataArea;
        PlotRegion* topMargin;
        PlotRegion* bottomMargin;
        PlotRegion* leftMargin;
        PlotRegion* rightMargin;
        PlotRegion* topAxis;
        PlotRegion* bottomAxis;
        PlotRegion* leftAxis;
        PlotRegion* rightAxis;

        int topM,bottomM,leftM,rightM;
        int topA,bottomA,leftA,rightA; 

        float minX;
        float minY;
        float maxX;
        float maxY;
        QGraphicsTextItem* label;
};

#endif
