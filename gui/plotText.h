#ifndef PLOT_TEXT_H
#define PLOT_TEXT_H

#include <QObject>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QFlags>


class PlotText : public QGraphicsTextItem {

    public:
        PlotText(QGraphicsItem* parent=0, QGraphicsScene* scene=0):QGraphicsTextItem(parent,scene) {};

        PlotText(QString l, QGraphicsItem* parent=0, QGraphicsScene* scene=0):QGraphicsTextItem(parent,scene) {
            setPlainText(l);
        }

    protected:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *widget){
            painter->setRenderHint(QPainter::TextAntialiasing, false);
            QGraphicsTextItem::paint(painter, option, widget);
        }
};

#endif
