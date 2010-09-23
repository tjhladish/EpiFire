#include "plotView.h"
#include "plotScene.h"
#include "plotPoint.h"
#include <math.h>
#include "../src/Utility.h"

PlotView::PlotView(QWidget*, QString title, QString xlabel, QString ylabel) {
    setObjectName("PlotView");
    setMinimumSize(500,200);

    rangeMin = -1;  // these are only used if the gui user
    rangeMax = -1;  // sets them somewhere
    Nbins    = -1;  //
    cutoff   = -1;  //

    myscene = new PlotScene(this);
    myscene->setSceneRect(0,0,width(),height()); //set scene to parent widget width x height
    setScene(myscene);
    clearPlot(); //initialize plotRegions, axes, and labels
    myscene->setTitle(title);
    myscene->setXLabel(xlabel);
    myscene->setYLabel(ylabel);

    setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    savePlotAction = new QAction("Export plot as png", this);
    connect( savePlotAction, SIGNAL(triggered()), this, SLOT(savePlot()) );
    saveDataAction = new QAction("", this);
    connect( saveDataAction, SIGNAL(triggered()), this, SLOT(saveData()) );
}

void PlotView::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
 
    if ( plotType == CURVEPLOT ) {
        saveDataAction->setText("Export time series data");
    } else if (plotType == STATEPLOT ) { 
        saveDataAction->setText("Export node state data (100 node max)");
    } else if (plotType == HISTPLOT or plotType == RESULTS_HISTPLOT) {
        saveDataAction->setText("Export epidemic size data");
    } else if ( plotType == DEGPLOT) { 
        saveDataAction->setText("Export degree sequence data");
    }
   

    menu.addAction(savePlotAction);
    menu.addAction(saveDataAction);
    menu.exec(event->globalPos());
}


void PlotView::replot() {

    if ( plotType == CURVEPLOT ) {
        drawEpiCurvePlot();
    } else if (plotType == STATEPLOT ) { 
        drawNodeStatePlot();
    } else if (plotType == HISTPLOT || plotType == DEGPLOT || plotType == RESULTS_HISTPLOT) { 
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


void PlotView::debugger() { // makes it easier to see what's going on with coordinates & plot area
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


int PlotView::default_nbins(double min_val, double max_val) {
    if (min_val < 0 or max_val < 0 or min_val > max_val) {
        cerr << "PlotView::default_nbins() must be called with valid min and max arguments.\n"
             << "Arguments given: " << min_val << ", " << max_val << endl;
        return 0;
    }
    int n = data[0].size();
    // for n data points, number of bins should be:
    // n if n < 10
    // 10 if 10 < n < 100
    // (int) sqrt(n) if n > 100
    int nbins = n < 10 ? n 
              : sqrt(n) < 10 ? 10
              : sqrt(n) < 20 ? sqrt(n)
              : 20;
    //nbins = sqrt(n) > 10 ? (int) sqrt(n) : nbins;
    
    float range = max_val - min_val;
   
    nbins = nbins > range + 1 ? range + 1 : nbins;
    return nbins;
}


vector<double> PlotView::default_minmax() {
   vector<double> minmax(2);
   minmax[0] = (double) min_element(data[0]);
   minmax[1] = (double) max_element(data[0]);
   return minmax;
}


void PlotView::drawHistogram() {
    clearPlot();

    if (data.size() == 0 || data[0].size() == 0) return;
    vector<int>& epi_data = data[0];

    int min_val = 0;
    int max_val = 0;
    int nbins   = 0;

    if (plotType == RESULTS_HISTPLOT) {
        vector<double> minmax = default_minmax();
        min_val = rangeMin == -1 ? minmax[0] : rangeMin;
        max_val = rangeMax == -1 ? minmax[1] : rangeMax;
        if ( max_val < min_val ) {
            min_val = minmax[0];
            max_val = minmax[1];
        }
        
        nbins = Nbins < 1 ? default_nbins(min_val, max_val) : Nbins;
    } else {
        vector<double> minmax = default_minmax();
        min_val = minmax[0];
        max_val = minmax[1];
        nbins = default_nbins(min_val, max_val);
    }

    vector<int> density(nbins,0);
    if (max_val == min_val) {
        for (unsigned int i = 0; i<epi_data.size(); i++) {
            if (epi_data[i] == max_val) density[0]++;
        }
    } else {
        for (unsigned int i = 0; i<epi_data.size(); i++) {
            int bin;
            if (epi_data[i] < min_val or epi_data[i] > max_val) {
                continue;
            } else if ( epi_data[i] == max_val ) { // last bin includes upper bound
                bin = nbins-1;
            } else {
                bin = nbins * ((double) epi_data[i]-min_val) / (max_val-min_val);
            }
            density[ bin ]++;
        }
    }

    int max_ct = max_element(density);

    Axis* xAxis = myscene->xAxis;
    xAxis->setNumTicks(nbins);
    xAxis->setRange(min_val,max_val);
    xAxis->forceNumTicks(true);
    xAxis->show(); 

    Axis* yAxis = myscene->yAxis;
    int yticks = max_ct > 10 ? 10 : max_ct;
    yAxis->preferedNumTicks(yticks);
    yAxis->useIntLabels(true);
    yAxis->calculateRange(0,max_ct);   
    yAxis->show(); 

    myscene->setXrange(0,nbins);
    myscene->setYrange(0,yAxis->getMax());

    QPen pen(Qt::white);
    QBrush brush(Qt::red);
    float w = (float) myscene->dataArea->width()/nbins;
    for (unsigned int r = 0; r < density.size(); r++) {
        float x = myscene->dataArea->toPlotX(r);
        float y = myscene->dataArea->toPlotY(density[r]);
        float h = myscene->dataArea->toPlotY(0) - y;
        if (cutoff < min_val + r*(max_val-min_val)/nbins) {
            brush.setColor(Qt::red);
        } else {
            brush.setColor(QColor(255,221,0));
        }
       
        QGraphicsRectItem* rect = new QGraphicsRectItem((qreal) x,(qreal) y,(qreal) w,(qreal) h);
        rect->setBrush(brush);
        rect->setPen(pen);
        rect->setParentItem(myscene->dataArea);
    }
}
    
 
void PlotView::drawEpiCurvePlot() {
    if (data.size() == 0 ) {
        clearPlot();
        return;
    }
        
    setRenderHint(QPainter::Antialiasing); // smooth data points

    float max_val = (float) find_max_val(data);
    int   max_idx = find_max_idx(data);
    
    epiCurveAxisUpdated((double) max_idx);

    Axis* xAxis = myscene->xAxis;
    xAxis->useIntLabels(true);
    xAxis->calculateRange(0,max_idx);
    xAxis->show(); 
    
    Axis* yAxis = myscene->yAxis;
    yAxis->useIntLabels(true);
    yAxis->calculateRange(0,max_val);   
    yAxis->show(); 

    myscene->setXrange(0,xAxis->getMax());
    myscene->setYrange(0,yAxis->getMax());

    float r = 3;                 // radius of data points
    qreal zval = 0;
    
    int alpha;
    int alpha_threshold[2] = {2, 25};
    if ((signed) data.size() < alpha_threshold[0]) {
        alpha = 255;
    }
    else if ((signed) data.size() > alpha_threshold[1]) {
        alpha = 10;
    }
    else {
        alpha = 255 / data.size();
    }

    QColor default_color(0,0,0,alpha);
    QBrush brush(default_color);

    foreach(QGraphicsItem* item, myscene->dataArea->childItems() ) {
        if (Point* p = qgraphicsitem_cast<Point*>(item)) {
            p->setBrush(brush);
            p->updatePosition();
        }
    }

    QColor recent_color = Qt::red;
    for( int i = newDataCursor; i < (signed) data.size(); i++ ) {
        if (i == (signed) data.size() - 1) {
            brush.setColor(recent_color);
            zval = 1;
        }
        for( unsigned int j=0; j<data[i].size(); j++ ) {
            Point*  dot = new Point( j, data[i][j] , r);
            dot->setParentItem(myscene->dataArea);
            dot->setBrush(brush);
            dot->updatePosition();
            dot->setZValue(zval);
        }
    }
    
    newDataCursor = data.size();
    myscene->update();

}

void PlotView::drawNodeStatePlot() {

    clearPlot();
    QRgb colors[4] = { qRgb(0, 0, 200), qRgb(254, 0, 0), qRgb(254, 254, 0), qRgb(254,254,254) };

    if (data.size() == 0 ) {
        clearPlot();
        return;
    }
    
    int node_ct = (signed) data[0].size();
    int duration = (signed) data.size() - 1;
    int xmax = rangeMax > duration? rangeMax : duration;

    Axis* xAxis = myscene->xAxis;
    xAxis->useIntLabels(true);
    xAxis->calculateRange(0,xmax);
    if (xmax == xAxis->getMax() and xmax == duration) xAxis->calculateRange(0, xmax+1);
    
    xAxis->show(); 
    xmax = xAxis->getMax();


    
    Axis* yAxis = myscene->yAxis;
    yAxis->useIntLabels(true);
    yAxis->calculateRange(0,node_ct);   
    yAxis->show(); 

    myscene->setXrange(0,xAxis->getMax());
    myscene->setYrange(0,yAxis->getMax());

    QRgb value;
    QImage image(xmax,  node_ct,QImage::Format_ARGB32);
    image.fill(Qt::white);
    for( int x=0; x < (signed) data.size(); x++) {
        for( int y=0; y < (signed) data[x].size(); y++ ) {
             int val = data[x][y];
             
             if (val == 0) {}
             else if (val == -1) { val = 2; }
             else { val = 1;}

             if (val > 2) cerr << "Node " << x << " has nonsense state " << val << endl;
             value = colors[ val % 4 ];
             image.setPixel(x, y, value);
        }
    }

    int W = myscene->dataArea->width();
    int H = myscene->dataArea->height();
    image = image.scaled(W,H);

    QPixmap pixmap = QPixmap::fromImage(image,Qt::AutoColor);
    QGraphicsPixmapItem* mypixmap  = new QGraphicsPixmapItem(pixmap);
    mypixmap->setParentItem(myscene->dataArea);

}


void PlotView::resizeEvent ( QResizeEvent * ) {
    newDataCursor = 0;
    myscene->setSceneRect(0,0,width(),height()); //set scene to parent widget width x height
    clearPlot();
    replot();
}


void PlotView::addData( vector<int> X ) { 
    if (plotType == STATEPLOT or plotType == CURVEPLOT) {
        data.push_back(X);
    } else {
        if (data.empty()) {
            vector<int> nothing;
            data.push_back(nothing);
        }
        data[0].insert(data[0].end(), X.begin(), X.end());
    }
}

void PlotView::clearPlot() { 
    myscene->clearPlot();
}


void PlotView::clearData() {
    data.clear();
    recentDataCursor = 0;
    newDataCursor = 0;
}

void PlotView::saveData() {
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
    } else if (plotType == HISTPLOT || plotType == DEGPLOT || plotType == RESULTS_HISTPLOT) {
        // One number per line
         for( unsigned int r=0; r < data.size(); r++) {
            for( unsigned int c=0; c < data[r].size(); c++ ) {
                out << data[r][c] << endl;
            }
        }
    }

    file.close();

}


void PlotView::savePlot() {
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
