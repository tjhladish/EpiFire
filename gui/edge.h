#ifndef EDGE_H
#define EDGE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOption>
#include <QObject>
#include <QPainter>
#include <QGraphicsItem>
#include <math.h>
#include <string>
#include "node.h"


const double Pi=3.14;

class GNode;
class Reaction;
using namespace std;

class GEdge : public QObject, public QGraphicsItem
{
Q_OBJECT
   
#if QT_VERSION >= 0x040600
    Q_INTERFACES( QGraphicsItem )
#endif

public:
    GEdge();
    ~GEdge();

    GNode *sourceGNode() const;
    void setSourceGNode(GNode *node);

    GNode *destGNode() const;
    void setDestGNode(GNode *node);

    void adjust();

    enum { Type = UserType + 2 };
    int type() const { return Type; }
 	inline void setNote(QString note)  {  _note = note; }
	inline void setData(void* data)  {   _data = data; }
	inline void setReversable(bool t) { _reversable = t; }
	inline bool isReversable() { return _reversable; }
	inline void setArrowSize(qreal a)  { _arrowSize = a; }
	inline void setColor(QColor c)     { _color=c; }

    float computeArrowSize(float flux);

	QColor		getColor()    		   { return _color; }
	qreal getArrowSize() { return _arrowSize; }

	double 	angle();
	double	length();
	void reverseDirection();
   
protected:
    QRectF boundingRect() const;
	QPainterPath shape () const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	void drawArrow(QPainter* painter);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
	void hoverEnterEvent (QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent*);
 
    
private:
    GNode *source, *dest;
    QPointF sourcePoint;
    QPointF destPoint;
	QString _note;
	QColor _color;
    qreal _arrowSize;
	bool _reversable;
	void* _data;

	QPainterPath _shape;
	
    double computeAngle(const QPointF& a, const QPointF& b);
};

#endif
