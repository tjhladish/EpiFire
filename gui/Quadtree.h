#ifndef QUADTREE_H
#define QUADTREE_H
#include <limits>
#include <math.h>
#include <vector>
#include <iostream>
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

struct Particle;

struct Link {
    Particle* source;
    Particle* dest;
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

    Particle (double _x, double _y ) : x(_x), y(_y), vx(0), vy(0), fx(0), fy(0) {}
    int totalDegree() { return linksIn.size() + linksOut.size(); }
};

struct QuadtreeNode {
    bool leaf = true;
    QuadtreeNode* c1 = nullptr;
    QuadtreeNode* c2 = nullptr;
    QuadtreeNode* c3 = nullptr;
    QuadtreeNode* c4 = nullptr;
    Particle*     p  = nullptr;

    double cx = 0;
    double cy = 0;
    double cn = 0;

    void print() {
        if(p) std::cerr << "P[xy] = " << p->x << "," << p->y << std::endl;
        if(c1) c1->print();
        if(c2) c2->print();
        if(c3) c3->print();
        if(c4) c4->print();
    }

    ~QuadtreeNode() { 
        if(c1) delete(c1);
        if(c2) delete(c2);
        if(c3) delete(c3);
        if(c4) delete(c4);
    }
};

struct Quadtree {
    double xMin;
    double xMax;
    double yMin;
    double yMax;
    QuadtreeNode* root;

    Quadtree(std::vector<Particle*>& particles) {
        /* Compute bounds. */
        double x1 = std::numeric_limits<double>::max();
        double y1 = x1;
        double x2 = std::numeric_limits<double>::min();
        double y2 = x2;

        for (auto p: particles) {
            if (p->x < x1) x1 = p->x;
            if (p->y < y1) y1 = p->y;
            if (p->x > x2) x2 = p->x;
            if (p->y > y2) y2 = p->y;
        }

        /* Squarify the bounds. */
        double dx = x2 - x1;
        double dy = y2 - y1;
        if (dx > dy) {
            y2 = y1 + dx;
        } else { 
            x2 = x1 + dy;
        }
        xMin = x1;
        yMin = y1;
        xMax = x2;
        yMax = y2;


        /* Insert all particles. */
        root = new QuadtreeNode();
        for (auto p: particles) insert(this->root, p, x1, y1, x2, y2);

        //root->print();
    }

    ~Quadtree() { 
        if(root) delete(root);
     }

    /**
     * @ignore Recursively inserts the specified particle <i>p</i> into a
     * descendant of node <i>n</i>. The bounds are defined by [<i>x1</i>,
     * <i>x2</i>] and [<i>y1</i>, <i>y2</i>].
     */
    void insertChild( QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2 ) {
        /* Compute the split point, and the quadrant in which to insert p. */
        double sx = (x1 + x2) * .5;
        double sy = (y1 + y2) * .5;
        bool  right = p->x >= sx;
        bool bottom = p->y >= sy;

        /* Recursively insert into the child node. */
        n->leaf = false;
        switch (((int) bottom << 1) + (int) right) {
            case 0: n = n->c1 ? n->c1 : (n->c1 = new QuadtreeNode()); break;
            case 1: n = n->c2 ? n->c2 : (n->c2 = new QuadtreeNode()); break;
            case 2: n = n->c3 ? n->c3 : (n->c3 = new QuadtreeNode()); break;
            case 3: n = n->c4 ? n->c4 : (n->c4 = new QuadtreeNode()); break;
        }

        /* Update the bounds as we recurse. */
        if (right) x1 = sx; else x2 = sx;
        if (bottom) y1 = sy; else y2 = sy;
        insert(n, p, x1, y1, x2, y2);
    }

    /**
     * @ignore Recursively inserts the specified particle <i>p</i> at the node
     * <i>n</i> or one of its descendants. The bounds are defined by [<i>x1</i>,
     * <i>x2</i>] and [<i>y1</i>, <i>y2</i>].
     */
    void insert( QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2 ) {
        //std::cerr << "insert: " << n << " " << p->x <<"," << p->y << std::endl;
        if (n->leaf) {
            if (n->p) {
                /*
                 * If the particle at this leaf node is at the same position as the new
                 * particle we are adding, we leave the particle associated with the
                 * internal node while adding the new particle to a child node. This
                 * avoids infinite recursion.
                 */
                if ((fabs(n->p->x - p->x) + fabs(n->p->y - p->y)) < .01) {
                    insertChild(n, p, x1, y1, x2, y2);
                } else {
                    Particle* v = n->p;
                    n->p = nullptr;
                    insertChild(n, v, x1, y1, x2, y2);
                    insertChild(n, p, x1, y1, x2, y2);
                }
            } else {
                n->p = p;
            }
        } else {
            insertChild(n, p, x1, y1, x2, y2);
        }
    }
};

#endif
