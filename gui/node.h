#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QGraphicsItem>
#include <QList>
#include <QMenu>
#include <string>

#include "graphwidget.h"
#include "edge.h"

#define POW2(x) ((x)*(x))

class TinyPlot;
class GEdge;
class QGraphicsSceneMouseEvent;
class GraphWidget;

class GNode : public QObject, public QGraphicsItem
{
	Q_OBJECT
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
	GNode(QGraphicsItem* parent, QGraphicsScene *scene);
	~GNode();
	enum { Type = UserType + 10 };
	int type() const { return Type; };

    void addGEdge(GEdge *edge);
	void removeGEdge(GEdge* edge);
    inline QList<GEdge *> edges() { return edgeList; }
    QList<GEdge *> edgesIn();
    QList<GEdge *> edgesOut();
    QList<GEdge *> findConnectedGEdges(GNode* other);
    int totalDegree() { return edgeList.size(); }
	
    inline int getId()		{ return _id; }
    void setId(int v)			  { _id = v; }

	void setFixedPosition(bool flag)  { _fixedPosition=flag; }
	inline bool  isFixedPosition()	{ return _fixedPosition; }


    void setGNodeSize(float x) {_nodeSize=x; }
	float getGNodeSize() { return _nodeSize; }
    QPointF centerPoint()     {return pos(); }

    void setBrush(QBrush x) { _brush=x; }

	void setBoundingBox(int w, int h ) { _boxWidth=w; _boxHeight=h; }
    QRectF boundingRect() const;

	void setGraphWidget(GraphWidget *g);
	GraphWidget* getGraphWidget();

	QRect getTextRect(const QString text, float fontsize);
	float getTextWidth();

	bool setNewPos(float x, float y);

public slots:

signals:
	 void nodeMoved ( GNode* );
	 void nodePressed ( GNode* );
	 void nodeDoubleClicked ( GNode* );

protected:
    QVariant itemChange ( GraphicsItemChange change, const QVariant &value );
    void paint ( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
	void mousePressEvent ( QGraphicsSceneMouseEvent *event );
	void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent *event );

	GraphWidget* _graph;

private:
	void paint(QPainter*);
	   
private:
    QList<GEdge *> edgeList;
	QString _note;
    int _id;
    //void*  _data;

    QPointF newPos;
    QBrush  _brush;

	bool _fixedPosition;

    //node size
    float _nodeSize;

	//boundaries of box 
	int _boxWidth;
	int _boxHeight;

};

#endif
