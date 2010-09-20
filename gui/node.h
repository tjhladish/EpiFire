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

	enum   MolClass { Unassigned=0, Enzyme=1, Metabolite=2, Cofactor=3};
	inline MolClass molClass()		{ return _class; }
	void   setMolClass(MolClass v)		  { _class = v; }

    void addGEdge(GEdge *edge);
	void removeGEdge(GEdge* edge);
    inline QList<GEdge *> edges() { return edgeList; }
    QList<GEdge *> edgesIn();
    QList<GEdge *> edgesOut();
    QList<GEdge *> findConnectedGEdges(GNode* other);
	
	inline bool isHighlighted() { return _hilighted; }
	void setHighlighted(bool x) { _hilighted = x; }

	double computeGNodeSize(float concentration);

	inline const QString getNote()		{ return _note; }
	void setNote(QString v)			  { _note = v; }

	inline const QString getId()		{ return _id; }
	void setId(QString v)			  { _id = v; }

	inline void* getDataReference()			{ return _data; }
	void setDataReference(void* v) 			  { _data = v; }
    //void setData( int key, const QVariant & value){ QGraphicsItem:setData(key,value); }

	void setFixedPosition(bool flag)  { _fixedPosition=flag; }
	inline bool  isFixedPosition()	{ return _fixedPosition; }


    void setGNodeSize(float x) {_nodeSize=x; }
	float getGNodeSize() { return _nodeSize; }
    QPointF centerPoint()     {return pos(); }

    void calculateForces();
    bool advance();
    void adjustNeighbors();

	void setDepth(int d) { _depth=d; }
	int  getDepth() { return _depth; }

    void setBrush(QBrush x) { _brush=x; }

	void setBoundingBox(int w, int h ) { _boxWidth=w; _boxHeight=h; }
	QRectF boundingRect() const;
    //QPainterPath shape() const;

	void setGraphWidget(GraphWidget *g);
	GraphWidget* getGraphWidget();

	QRect getTextRect(const QString text, float fontsize);
	float getTextWidth();
	static bool compDepth(GNode* n1, GNode*n2) { return n1->getDepth() < n2->getDepth(); }

	bool setNewPos(float x, float y);

public slots:

signals:
	 void nodeMoved(GNode*);
	 void nodePressed(GNode*);
	 void nodeDoubleClicked(GNode*);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

	GraphWidget* _graph;

private:
	void paint(QPainter*);
	   
private:
    QList<GEdge *> edgeList;
	QString _note;
	QString _id;
	MolClass _class;
	void*  _data;

    QPointF newPos;
    QBrush  _brush;

	bool _hilighted;

	bool _fixedPosition;
	int _depth;

    //node size
    float _nodeSize;

	//boundaries of box 
	int _boxWidth;
	int _boxHeight;

};

#endif
