#ifndef QUADTREE_H
#define QUADTREE_H
#include <limits>
#include <math.h>
#include <vector>
#include <iostream>
#include "node.h"
#include "edge.h"
#include <QVector>

/**
 * Constructs a new quadtree for the specified array of particles.
 *
 * @class Represents a quadtree: a two-dimensional recursive spatial
 * subdivision. This particular implementation uses square partitions, dividing
 * each square into four equally-sized squares. Each particle exists in a unique
 * node; if multiple particles are in the same position, some particles may be
 * stored on internal nodes rather than leaf nodes.
 *
 * <p>This quadtree can be used to accelerate various spatial operations, such
 * as the Barnes-Hut approximation for computing n-body forces, or collision
 * detection.
 *
 * @see pv.Force.charge
 * @see pv.Constraint.collision
 * @param {pv.Particle} particles the linked list of particles.
 */

class GNode;
typedef GNode Particle;

class GEdge;
typedef GEdge Link;
//class Particle;


/*class Link {
  private:
    Particle* _source;
    Particle* _dest;
  public:
    Particle* source() { return _source; }
    Particle* dest() { return _dest; }
    Link (Particle* s, Particle* d):_source(s), _dest(d) {}
};

class Particle {
  private:
    double _x;
    double _y;
    QVector<Link*> _edgesIn;
    QVector<Link*> _edgesOut;

  public:
    double vx;
    double vy;
    double fx;
    double fy;

    double px;  //previous x and y positions
    double py;
    double x() const { return _x; }
    double y() const { return _y; }
    void setPos(double x, double y) { _x = x; _y = y; }
    QVector<Link*>* edgesIn() { return &_edgesIn; }
    QVector<Link*>* edgesOut() { return &_edgesOut; }

    Particle (double xi, double yi ) : _x(xi), _y(yi), vx(0), vy(0), fx(0), fy(0), px(xi), py(yi) {}
    ~Particle() { for (auto e: _edgesIn) delete e; for (auto e: _edgesOut) delete e; }
    int totalDegree() { return _edgesIn.size() + _edgesOut.size(); }
};*/

class QuadtreeNode {
  public:
    bool leaf = true;
    QuadtreeNode* c1 = nullptr;
    QuadtreeNode* c2 = nullptr;
    QuadtreeNode* c3 = nullptr;
    QuadtreeNode* c4 = nullptr;
    Particle*     p  = nullptr;

    double cx = 0;
    double cy = 0;
    double cn = 0;

/*    void print() {
        if(p) std::cerr << "P[xy] = " << p->x() << "," << p->y() << std::endl;
        if(c1) c1->print();
        if(c2) c2->print();
        if(c3) c3->print();
        if(c4) c4->print();
    }*/

    ~QuadtreeNode() { 
        if(c1) delete(c1);
        if(c2) delete(c2);
        if(c3) delete(c3);
        if(c4) delete(c4);
    }
};

class Quadtree {
  public:
    double xMin;
    double xMax;
    double yMin;
    double yMax;
    QuadtreeNode* root;

    Quadtree(QVector<Particle*>& particles);
    ~Quadtree() { if(root) delete(root); }

    void insertChild( QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2 );
    void insert( QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2 );
};

#endif
