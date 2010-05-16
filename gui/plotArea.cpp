#include "plotArea.h"
#include "plotScene.h"
#include "plotPoint.h"

#include "../src/Utility.h"

PlotArea::PlotArea(QWidget*) {
    setObjectName("PlotArea");
    //setMinimumSize(300,200);
    PlotScene* myscene = new PlotScene(this);
    setScene(myscene);
    scene()->setSceneRect(0,0,300,200);
    xAxis = NULL;
    yAxis = NULL;

}


void PlotArea::replot() {

    if ( plotType == EPICURVE ) {
        drawEpiCurvePlot();
    } else if (plotType == STATEPLOT ) { 
        drawNodeStatePlot();
    }
}


int find_max_val(vector< vector <int> > data) {
    int maxY = 0;
    for( unsigned int i=0; i < data.size(); i++ ) {
        for( unsigned int j=0; j < data[i].size(); j++ ) {
            int test_max = data[i][j];
            if (test_max > maxY) {
                maxY = test_max;
            }
        }
    }
    return maxY;
}


int find_max_idx(vector< vector <int> > data) {
    int maxX = 0;
    for( unsigned int i=0; i < data.size(); i++ ) {
        int test_max = data[i].size() - 1;
        if (test_max > maxX) {
            maxX = test_max;
        }
    }
    return maxX;
}


void PlotArea::debugger() { // makes it easier to see what's going on with coordinates & plot area
    // debugging data
        data.clear();
        vector<int> dummy;
        data.push_back(dummy);
        for (int i = 0; i < 10; i++) {
            data[0].push_back(i);
        }

    int plotW = width();
    int plotH = height();

    //debugging rectangles
        int S = 50;
        QColor tc(0,0,0,50);
        QBrush tb(tc);
        QPen   tp(Qt::black);
        scene()->addRect(0,0,S,S,tp,tb);
        scene()->addRect(0,plotH-S,S,S,tp,tb);
        scene()->addRect(plotW-S,0,S,S,tp,tb);
        scene()->addRect(plotW-S,plotH-S,S,S,tp,tb);
}

void PlotArea::clearPlot() { 
    scene()->clear();
    xAxis = new Axis(0,0.0,1.0,10);
    yAxis = new Axis(1,0.0,1.0,10);
    scene()->addItem(xAxis);
    scene()->addItem(yAxis);
    yAxis->translate(-40,0);
    xAxis->translate(0,+10);
    xAxis->hide();
    yAxis->hide();

}

void PlotArea::drawEpiCurvePlot() {
    setRenderHint(QPainter::Antialiasing); // smooth data points
    if (data.size() == 0 ) {
        clearPlot();
        scene()->update();
        return;
    }

    PlotScene* myscene = (PlotScene*) scene();

    //scene()->clear();
    int plotW = width() - 80;   //width of the view
    int plotH = height() - 50;  //height of the view
    myscene->setSceneRect(0,0,plotW,plotH);

    float axis_multiplier = 1.1; // how much longer should axes be
    float max_val = (float) find_max_val(data) * axis_multiplier;
    int   max_idx = find_max_idx(data) * axis_multiplier;


    myscene->setXrange(0,max_idx*axis_multiplier);
    myscene->setYrange(0,max_val*axis_multiplier);


    xAxis->setRange(0,max_idx*axis_multiplier);
    xAxis->show(); 
    yAxis->setRange(0,max_val*axis_multiplier);   
    yAxis->show(); 

    float r = 4;                 // radius of data points
    int margin = 35;
    qreal zval = 0;
    
    /*
    myscene->setXrange(0,10);
    myscene->setYrange(0,30);
    if(xAxis) { xAxis->setRange(0,10);  xAxis->show(); } 
    if(yAxis) { yAxis->setRange(0,30);   yAxis->show(); }
    
    Point*  p1 = new Point(0,0,10);
    Point*  p2 = new Point(5,15,10);
    Point*  p3 = new Point(10,30,10);

    p1->setBrush(Qt::red);
    p2->setBrush(Qt::blue);
    p3->setBrush(Qt::green);

    myscene->addItem(p3);
    myscene->addItem(p1);
    myscene->addItem(p2);

    p1->updatePosition();
    p2->updatePosition();
    p3->updatePosition();

    cerr << "WxH=" << scene()->width()  << " " << scene()->height() << endl;
    cerr << myscene->toPlotX(0) << " " << myscene->toPlotY(0) << endl;
    cerr << myscene->toPlotX(5) << " " << myscene->toPlotY(15) << endl;
    cerr << myscene->toPlotX(10) << " " << myscene->toPlotY(30) << endl;

    //PlotScene* myplot = (PlotScene*) scene();
    //setPos( myplot->toPlotX(x), myplot->toPlotY(y));
    
    return;
    */


    int alpha;
    int alpha_threshold[2] = {2, 25};
    if (data.size() < alpha_threshold[0]) {
        alpha = 255;
    }
    else if (data.size() > alpha_threshold[1]) {
        alpha = 10;
    }
    else {
        alpha = 255 / data.size();
    }

    QColor default_color(0,0,0,alpha);
    QBrush brush(default_color);

    foreach(QGraphicsItem* item, items() ) {
        if (Point* p = qgraphicsitem_cast<Point*>(item)) {
            p->setBrush(brush);
            p->updatePosition();
        }
    }

    QColor recent_color = Qt::red;
    for( int i = newDataCursor; i < (signed) data.size(); i++ ) {
        if (i == data.size() - 1) {
            brush.setColor(recent_color);
            zval = 1;
        }
        for( unsigned int j=0; j<data[i].size(); j++ ) {
            Point*  dot = new Point( j, data[i][j] , r);
            myscene->addItem(dot);
            dot->setBrush(brush);
            dot->updatePosition();
            dot->setZValue(zval);
        }
    }
    
    newDataCursor = data.size();

    scene()->update();
    setAlignment(Qt::AlignRight);

}

void PlotArea::drawNodeStatePlot() {
    QRgb colors[3] = { qRgb(0, 0, 254), qRgb(254, 254, 0), qRgb(0, 254, 0) };

    if (data.size() == 0 ) {
        scene()->clear();
        scene()->update();
        return;
    }
    
    QRgb value;
    //cerr << "1width: " << (int) width() << " height: " << (int) height() << endl;
    //cerr << "scene w: " << scene()->width() << " scene h: " << scene()->height() << endl;
    setSceneRect(0,0,width()-20,height()-20); //set scene to parent widget width x height
    //cerr << "scene w: " << scene()->width() << " scene h: " << scene()->height() << endl;
    //QImage image(scene()->width(),scene()->height(),QImage::Format_ARGB32);
    QImage image(data.size(),data[0].size(),QImage::Format_ARGB32);
    image.fill(Qt::white);
    int w = width();

    cerr << "HM" << data.size() << endl;

    for( unsigned int r=0; r < data.size(); r++) {
        for( unsigned int c=0; c < data[r].size(); c++ ) {
        //for( unsigned int c=0; c < data[r].size()/2 && c < width; c++ ) {
             int val = data[r][c];
             
             if (val == 0) {}
             else if (val == -1) { val = 2; }
             else { val = 1;}

             //cerr << "HMM" << w << " " <<r << " " << c << " " << val << endl;
             if (val > 2) cerr << "Node " << c << " has nonsense state " << val << endl;
             value = colors[ val % 3 ];
             image.setPixel(r, c, value);
             //cerr << val;
        }
        //cerr << endl;
    }
    
    image = image.scaled(width()-20, height()-20);
    //cerr << "2width: " << (int) width() << " height: " << (int) height() << endl;
    QPixmap pixmap = QPixmap::fromImage(image,Qt::AutoColor);
    scene()->addPixmap(pixmap);
}


void PlotArea::resizeEvent ( QResizeEvent * event ) {
    newDataCursor = 0;
    clearPlot();
    replot();
}


void PlotArea::saveData() {
    QString startdir = ".";
    QString file = QFileDialog::getOpenFileName(
        this, "Select file to save to", startdir, "CSV Files(*.csv)");

}


void PlotArea::savePicture() {
    QString startdir = ".";
    QString file = QFileDialog::getOpenFileName(
        this, "Select file to save to", startdir, "PNG Image Files(*.png)");

    QPixmap image(scene()->width(),scene()->height());
    image.fill(Qt::white);

    QPainter painter(&image);
    render(&painter);

    /*clipboard*/
    //QApplication::clipboard()->setPixmap(image);

    image.save(file,"PNG");

}
