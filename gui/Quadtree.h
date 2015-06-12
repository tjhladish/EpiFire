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

/*struct Link {
    Particle* source;
    Particle* dest;
    Link (Particle* s, Particle* d):source(s), dest(d) {}
};

struct Particle { 
    double x; 
    double y;
    double vx;
    double vy;
    double fx;
    double fy;

    double px;  //previous x and y positions
    double py;
    std::vector<Link*> linksIn;
    std::vector<Link*> linksOut;

    Particle (double _x, double _y ) : x(_x), y(_y), vx(0), vy(0), fx(0), fy(0), px(_x), py(_y) {}
    ~Particle() { for (auto e: linksIn) delete e; for (auto e: linksOut) delete e; }
    int totalDegree() { return linksIn.size() + linksOut.size(); }
};*/
//double GNode::x();
//double GNode::y();
class GNode;

class QuadtreeNode {
  public:
    bool leaf = true;
    QuadtreeNode* c1 = nullptr;
    QuadtreeNode* c2 = nullptr;
    QuadtreeNode* c3 = nullptr;
    QuadtreeNode* c4 = nullptr;
    GNode*        p  = nullptr;

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

    Quadtree(QVector<GNode*>& particles);
    ~Quadtree() { if(root) delete(root); }

    void insertChild( QuadtreeNode* n, GNode* p, double x1, double y1, double x2, double y2 );
    void insert( QuadtreeNode* n, GNode* p, double x1, double y1, double x2, double y2 );
};

#endif
