#include "plotArea.h"
#include "plotScene.h"
#include "plotPoint.h"
#include <math.h>
#include "../src/Utility.h"

PlotArea::PlotArea(QWidget*, QString l) {
    setObjectName("PlotArea");
    setMinimumSize(500,200);
    myscene = new PlotScene(this);
    setScene(myscene);
    scene()->setSceneRect(0,0,minimumWidth()-110,minimumHeight()-50);
    label = l;
    xAxis = NULL;
    yAxis = NULL;

    savePlotAction = new QAction("Export plot as png", this);
    connect( savePlotAction, SIGNAL(triggered()), this, SLOT(savePlot()) );
    saveDataAction = new QAction("", this);
    connect( saveDataAction, SIGNAL(triggered()), this, SLOT(saveData()) );
}

void PlotArea::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
 
    if ( plotType == CURVEPLOT ) {
        saveDataAction->setText("Export time series data");
    } else if (plotType == STATEPLOT ) { 
        saveDataAction->setText("Export node state data (100 node max)");
    } else if (plotType == HISTPLOT) {
        saveDataAction->setText("Export epidemic size data");
    } else if ( plotType == DEGPLOT) { 
        saveDataAction->setText("Export degree sequence data");
    }
   

    menu.addAction(savePlotAction);
    menu.addAction(saveDataAction);
    menu.exec(event->globalPos());
}


void PlotArea::replot() {

    if ( plotType == CURVEPLOT ) {
        drawEpiCurvePlot();
    } else if (plotType == STATEPLOT ) { 
        drawNodeStatePlot();
    } else if (plotType == HISTPLOT || plotType == DEGPLOT) { 
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
    // Args: axis_type, min, max, nticks, force_nticks, use_int_labels
    xAxis = new Axis(0, 0.0, 1.0, 10, false, false);
    yAxis = new Axis(1, 0.0, 1.0, 10, false, false);
    scene()->addItem(xAxis);
    scene()->addItem(yAxis);
    yAxis->translate(-40,0);
    xAxis->translate(0,+10);
    xAxis->hide();
    yAxis->hide();
    myscene->setLabel(label, 0, -30);
}


void PlotArea::drawHistogram() {
    setRenderHint(QPainter::Antialiasing); // smooth data points
    clearPlot();
    scene()->update(); // clears any artifacts in plot margins
    if (data.size() == 0 ) return;
    
    PlotScene* myscene = (PlotScene*) scene();

    int plotW = width() - 110;   //width of the view
    int plotH = height() - 80;  //height of the view
    myscene->setSceneRect(0,0,plotW,plotH);

    int n = 0;
    for (unsigned int i =0; i < data.size(); i++) n += data[i].size();

    // for n data points, number of bins should be:
    // n if n < 10
    // 10 if 10 < n < 100
    // (int) sqrt(n) if n > 100
    int nbins = n < 10 ? n 
              : sqrt(n) < 10 ? 10
              : sqrt(n) < 20 ? sqrt(n)
              : 20;
    //nbins = sqrt(n) > 10 ? (int) sqrt(n) : nbins;
    
    float max_val = (float) find_max_val(data);
    float min_val = (float) find_min_val(data);
    float range = max_val - min_val;
   
    nbins = nbins > range + 1 ? range + 1 : nbins;

    vector<int> density(nbins,0);
    if (max_val == min_val) {
        density[0] = n;
    } else {
        for (unsigned int i = 0; i<data.size(); i++) {
            for (unsigned int j = 0; j<data[i].size(); j++) {
                int bin = (data[i][j]-min_val) / (max_val-min_val)*(nbins-1);
                density[ bin ]++;
            }
        }
    }

    int max_ct = max_element(density);

    xAxis->setLabel("Epidemic size");
    xAxis->setNumTicks(nbins);
    xAxis->setRange(min_val,max_val);
    xAxis->forceNumTicks(true);
    xAxis->show(); 

    yAxis->setLabel("Frequency");
    int yticks = max_ct > 10 ? 10 : max_ct;
    yAxis->preferedNumTicks(yticks);
    yAxis->calculateRange(0,max_ct);   
    yAxis->useIntLabels(true);
    yAxis->show(); 

    myscene->setXrange(0,nbins);
    myscene->setYrange(0,yAxis->getMax());

    QPen pen(Qt::white);
    QBrush brush(Qt::red);
    float w = (float) myscene->width()/nbins;
    for (unsigned int r = 0; r < density.size(); r++) {
        float x = myscene->toPlotX(r);
        float y = myscene->toPlotY(density[r]);
        float h = myscene->toPlotY(0) - y;
        myscene->addRect((qreal) x,(qreal) y,(qreal) w,(qreal) h, pen, brush);
    }
}
    
 
void PlotArea::drawEpiCurvePlot() {
    setRenderHint(QPainter::Antialiasing); // smooth data points
    if (data.size() == 0 ) {
        clearPlot();
        scene()->update();
        return;
    }

    PlotScene* myscene = (PlotScene*) scene();

    int plotW = width() - 110;   //width of the view
    int plotH = height() - 80;  //height of the view
    myscene->setSceneRect(0,0,plotW,plotH);

    float max_val = (float) find_max_val(data);
    int   max_idx = find_max_idx(data);

    xAxis->calculateRange(0,max_idx);
    xAxis->useIntLabels(true);
    xAxis->show(); 
    
    yAxis->calculateRange(0,max_val);   
    yAxis->useIntLabels(true);
    yAxis->show(); 

    myscene->setXrange(0,xAxis->getMax());
    myscene->setYrange(0,yAxis->getMax());

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

    myscene->update();
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

    scene()->setSceneRect(0,0,W-20,H-20); //set scene to parent widget width x height

    QImage image(data.size(),data[0].size(),QImage::Format_ARGB32);
    image.fill(Qt::white);
    int w = width();

    for( unsigned int r=0; r < data.size(); r++) {
        for( unsigned int c=0; c < data[r].size(); c++ ) {
             int val = data[r][c];
             
             if (val == 0) {}
             else if (val == -1) { val = 2; }
             else { val = 1;}

             if (val > 2) cerr << "Node " << c << " has nonsense state " << val << endl;
             value = colors[ val % 3 ];
             image.setPixel(r, c, value);
        }
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
    QString filename = QFileDialog::getSaveFileName(this, "Select file to save to", startdir, "CSV Files(*.csv)");

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);

    if ( plotType == CURVEPLOT ) {
        // One time series per line
        for( unsigned int r=0; r < data.size(); r++) {
            for( unsigned int c=0; c < data[r].size() - 1; c++ ) {
                out << data[r][c] << ",";
            }
            out << data[r][data[r].size()-1] << endl;
        }
    } else if (plotType == STATEPLOT ) { 
        // Swap rows and columns -- data structure must be rectangular!
        for( unsigned int r=0; r < data[0].size(); r++ ) {
            for( unsigned int c=0; c < data.size() - 1; c++) {
                out << data[c][r] << ",";
            }
            out << data[data.size()-1][r] << endl;
        }
    } else if (plotType == HISTPLOT || plotType == DEGPLOT) {
        // One number per line
         for( unsigned int r=0; r < data.size(); r++) {
            for( unsigned int c=0; c < data[r].size(); c++ ) {
                out << data[r][c] << endl;
            }
        }
    }

    file.close();

}


void PlotArea::savePlot() {
    QString startdir = ".";
    QString filename = QFileDialog::getSaveFileName(
        this, "Select file to save to", startdir, "PNG Image Files(*.png)");

//    QPixmap image(scene()->width(),scene()->height());
    QPixmap image(width(),height());
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    render(&painter);

    /*clipboard*/
    //QApplication::clipboard()->setPixmap(image);

    image.save(filename,"PNG");

}
