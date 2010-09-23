#ifndef PLOTSCENE_H
#define PLOTSCENE_H

#include <QtGui>
#include <QGraphicsScene>
#include <QVector4D>
#include <QVector2D>
#include "plotRegion.h"
#include "plotAxis.h"
#include "plotText.h"

class PlotRegion;
class PlotText;

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
        void clearPlot();

        float getMinX() { return minX; }
        float getMinY() { return minY; }
        float getMaxX() { return maxX; }
        float getMaxY() { return maxY; }

        void setXLabel(QString l) { xlabelText=l;}// xlabel->setPlainText(l); }
        void setYLabel(QString l) { ylabelText=l;}// ylabel->setPlainText(l); }
        void setTitle (QString l) { titleText=l;  }//title=(PlotText*) addText(l);  }

        void setMarginsDim(int top, int bottom, int left, int right) { topM=top; bottomM=bottom; leftM=left; rightM=right; reDefinePlotRegions(); }
        void setAxesDim(int top, int bottom, int left, int right) { topA=top; bottomA=bottom; leftA=left; rightA=right; reDefinePlotRegions(); }
        QVector4D getMarginDim() { return QVector4D(topM,bottomM,leftM,rightM); }
        QVector4D getAxesDim() { return QVector4D(topM,bottomM,leftM,rightM); }
        QVector2D getDataAreaDim();

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
        Axis* xAxis;
        Axis* yAxis;

        PlotText* xlabel;
        PlotText* ylabel;
        PlotText* title;

        int topM,bottomM,leftM,rightM;
        int topA,bottomA,leftA,rightA; 

        float minX;
        float minY;
        float maxX;
        float maxY;

        QString xlabelText;
        QString ylabelText;
        QString titleText;
};

#endif
