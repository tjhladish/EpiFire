#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>
#include <QPointer>
#include <QKeyEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "node.h"
#include "edge.h"
#include <string>

using namespace std;

class GNode;
class GEdge;

class MyScene : public QGraphicsScene
{
    Q_OBJECT

	public:
	MyScene(QObject * parent = 0): QGraphicsScene(parent) {};
	
	protected:
	void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );

	signals:
	void zoomArea(QRectF);

	private:
		QPointF down;
		QPointF up;
};


class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget();
	~GraphWidget();

	void clear();
    void itemMoved();
    GEdge* addGEdge(GNode* a, GNode* b, string id, void* data);
	GNode* addGNode(string id, void* data);
	GNode* locateGNode(QString id);
	void removeGNode(GNode* n);
	GEdge* findGEdge(GNode* n1, GNode* n2);

    void  setGNodeSizeScale( float scale) { _nodeSizeScale=scale; }
    float getGNodeSizeScale() { return _nodeSizeScale; }

	void  setLabelSizeScale( float scale) { _labelSizeScale=scale; }
    float getLabelSizeScale() { return _labelSizeScale; }

	void  setGEdgeSizeScale( float scale) { _edgeSizeScale=scale; }
    float getGEdgeSizeScale() { return _edgeSizeScale; }

	enum  sizeNormalization { FixedSize=1, AbsoluteSize=2, RelativeSize=3, PairwiseSize=4 };
	sizeNormalization getGNodeSizeNormalization() { return _nodeSizeNormalization; }

	enum  LayoutAlgorithm { FMMM=1, Circular=2, Balloon=3 };
	LayoutAlgorithm getLayoutAlgorithm() { return _layoutAlgorithm; }

    void  setTitle(const QString& title);

	QList<GNode*> getGNodes(int type);

public slots:
	void resetZoom();
    void zoomIn() { scale(1.2, 1.2); }
    void zoomOut() { scale(1 / 1.2, 1 / 1.2); }
	void zoomArea(QRectF sceneArea) { fitInView(sceneArea,Qt::KeepAspectRatio); }
	void increaseLabelSize() { setLabelSizeScale(getLabelSizeScale()*1.2); scene()->update(); }
	void decreaseLabelSize() { setLabelSizeScale(getLabelSizeScale()*0.8); scene()->update(); }
	void increaseGNodeSize() {  setGNodeSizeScale(getGNodeSizeScale()*1.2);   scene()->update(); }
	void decreaseGNodeSize() {  setGNodeSizeScale(getGNodeSizeScale()*0.8);   scene()->update(); }
	void increaseGEdgeSize() {  setGEdgeSizeScale(getGEdgeSizeScale()*1.2);   scene()->update(); }
	void decreaseGEdgeSize() {  setGEdgeSizeScale(getGEdgeSizeScale()*0.8);   scene()->update(); }

	void computeAvgGEdgeLength();
	float getAvgGEdgeLength() { return _averageGEdgeSize; }

	void  setLayoutAlgorithm(LayoutAlgorithm x) { _layoutAlgorithm = x; }
	void  setGNodeSizeNormalization(sizeNormalization x) { _nodeSizeNormalization = x; }


	void dump();
    void layoutOGDF();
    void randomLayout();
    void updateLayout();
    void newLayout();
	void adjustLayout();
    void adjustLayout(GNode*);
    void clearLayout();
	void removeSelectedGNodes();
    void addToTree(GNode* a, GNode* b);

protected:
	void updateSceneRect();
    void drawBackground(QPainter *painter, const QRectF &rect);
	void keyPressEvent(QKeyEvent *event);
	//void resizeEvent( QResizeEvent * ) { resetZoom(); }

    void scaleView(qreal scaleFactor);
	void randomGNodePositions();
	void recursiveDepth(GNode* n0,int depth);
	void recursiveDraw(QTreeWidgetItem* item);
	QPointer<QGraphicsTextItem> _title;

    QHash<QString,GNode*> nodelist;


private:
	//graph layout
    
	sizeNormalization _nodeSizeNormalization;
	LayoutAlgorithm	  _layoutAlgorithm;

	QTreeWidget* layoutTree;
	QHash<GNode*, QTreeWidgetItem*>layoutMap;
	void deepChildCount(QTreeWidgetItem* x, int* count);

	float _nodeSizeScale;
	float _labelSizeScale;
	float _edgeSizeScale;
	float _averageGEdgeSize;
    bool  _updateLayout;

};

#endif
