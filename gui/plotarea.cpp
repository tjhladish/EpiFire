#include "plotarea.h"


PlotArea::PlotArea(QWidget*) { 
    setScene(new QGraphicsScene(this));
    setObjectName("PlotArea");
    setMinimumSize(200,300);
}


void PlotArea::replot() {
    drawPlot();
}

void PlotArea::drawPlot() { 
    
    scene()->clear();  
    scene()->setSceneRect(0,0,width()-20,height()-20);
    //QBrush brush(Qt::red);
    int alpha;
    if (data.size() < 2) {
        alpha = 255;
    } else if (data.size() > 25) {
        alpha = 10;
    } else {
        alpha = 255 / data.size();
    }
    QColor color(0,0,0,alpha);
    QBrush brush(color);
    QPen   pen(color);

    QColor recent_color(255,0,0,255);
//    QPen   pen(Qt::black);
    //scene()->addRect(width()-1,height()-1,1,1,pen,brush);    
    int maxY=5000;
    int w=7;
    int h_padding = 3;
    int margin = 15;
    for( unsigned int i=0; i < data.size(); i++ ) {
        if (i == data.size() -1) {
            brush.setColor(recent_color);
            pen.setColor(recent_color);
        }
        for( unsigned int j=0; j<data[i].size(); j++ ) {
            int h= height() - ((float) data[i][j])/maxY * height();
            int x=j*(w+h_padding) + margin;
            //int y=scene()->height(); //bottom
            scene()->addEllipse(x,h-30,w,w,pen,brush);    
//            scene()->addRect(x,y,w,-h,pen,brush);    
            //qDebug() << data[i][j];
        }
    }
    qDebug() << width() << " " << height() << endl;
    //see QGraphicsScene
    
    
    
    
    
    scene()->update();
    
}

void PlotArea::resizeEvent ( QResizeEvent * event ) {
    replot();	 
}
