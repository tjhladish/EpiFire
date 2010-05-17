#include "plotArea.h"
#include "plotScene.h"
#include "plotPoint.h"

#include "../src/Utility.h"

PlotArea::PlotArea(QWidget*) {
    setObjectName("PlotArea");
    //setMinimumSize(300,200);
    PlotScene* myscene = new PlotScene(this);
    setScene(myscene);
    scene()->setSceneRect(0,0,400,100);
    xAxis = NULL;
    yAxis = NULL;

}


void PlotArea::replot() {

    if ( plotType == EPICURVE ) {
        drawEpiCurvePlot();
    } else if (plotType == STATEPLOT ) { 
        drawNodeStatePlot();
    } else if (plotType == HISTPLOT ) { 
        drawHistogram();
    }
}


int find_min_val(vector< vector <int> > data) {
    int minY = INT_MAX;
    for( unsigned int i=0; i < data.size(); i++ ) {
        for( unsigned int j=0; j < data[i].size(); j++ ) {
            int test_min = data[i][j];
            if (test_min < minY) {
                minY = test_min;
            }
        }
    }
    return minY;
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
void PlotArea::drawHistogram() {
    setRenderHint(QPainter::Antialiasing); // smooth data points
    clearPlot();
    if (data.size() == 0 ) {
        scene()->update();
        return;
    }
/*    clearData();
    vector<int> fake;
    fake.push_back(3);
    fake.push_back(3);
    fake.push_back(4);
    fake.push_back(7);
    fake.push_back(3);
    fake.push_back(6);
    fake.push_back(6);
    fake.push_back(4);
    data.push_back(fake);    
*/
    PlotScene* myscene = (PlotScene*) scene();

    //scene()->clear();
    int plotW = width() - 80;   //width of the view
    int plotH = height() - 50;  //height of the view
    myscene->setSceneRect(0,0,plotW,plotH);

    float axis_multiplier = 1.1; // how much longer should axes be

    int n = 0;
    for (unsigned int i =0; i < data.size(); i++) n += data[i].size();
    int nbins = n < 10 ? n : 10; 

    float max_val = (float) find_max_val(data);
    float min_val = (float) find_min_val(data);

    vector<int> density(nbins,0);
    cerr << "nbins: " << nbins << endl;
    if (max_val == min_val) {
        density[0] = n;
    } else {
        for (unsigned int i = 0; i<data.size(); i++) {
            for (unsigned int j = 0; j<data[i].size(); j++) {
                int bin = (data[i][j]-min_val) / (max_val-min_val)*(nbins-1);
                density[ bin ]++;
    cerr << data[i][j] << "<-datum bin->" << bin<< endl;
            }
        }
    }
    cerr << endl;

    for (unsigned int i = 0; i<density.size(); i++) {
        cerr << i << " " << density[i] << endl;
     }

    int   max_ct = max_element(density);

    myscene->setXrange(0,nbins);
    myscene->setYrange(0,max_ct);

    xAxis->setRange(min_val,max_val);
    xAxis->setNumTicks(nbins+1);
    xAxis->show(); 
    yAxis->setRange(0,max_ct);   
    yAxis->show(); 

    QPen pen(Qt::white);
    QBrush brush(Qt::red);
    //cerr << "scenew: " << myscene->width() << endl;
    float w = (float) myscene->width()/nbins;
    for (unsigned int r = 0; r < density.size(); r++) {
        float x = myscene->toPlotX(r);
        float y = myscene->toPlotY(density[r]);
        float h = myscene->toPlotY(0) - y;
        //cerr << "coords: " << x << " " << y << " " << w << " " << h << endl;
        myscene->addRect((qreal) x,(qreal) y,(qreal) w,(qreal) h, pen, brush);
    }
}
    
 
void PlotArea::drawEpiCurvePlot() {
    setRenderHint(QPainter::Antialiasing); // smooth data points
//clearData();
//cerr << "Data cleared for epi curve plot\n";
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

    float axis_multiplier = 1; // how much longer should axes be
    float max_val = (float) find_max_val(data) * axis_multiplier;
    int   max_idx = find_max_idx(data) * axis_multiplier;


    myscene->setXrange(0,max_idx*axis_multiplier);
    myscene->setYrange(0,max_val*axis_multiplier);


    xAxis->setRange(0,max_idx*axis_multiplier);
    xAxis->show(); 
    yAxis->setRange(0,max_val*axis_multiplier);   
    yAxis->show(); 

    float r = 3;                 // radius of data points
    int margin = 35;
    qreal zval = 0;
    
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
    //setAlignment(Qt::AlignRight);

}

void PlotArea::drawNodeStatePlot() {
    QRgb colors[3] = { qRgb(0, 0, 200), qRgb(254, 0, 0), qRgb(254, 254, 0) };

    if (data.size() == 0 ) {
        scene()->clear();
        scene()->update();
        return;
    }
    
    QRgb value;
    int W = width();
    int H = height();

    //cerr << "1width: " << (int) width() << " height: " << (int) height() << endl;
    scene()->setSceneRect(0,0,W-20,H-20); //set scene to parent widget width x height

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
