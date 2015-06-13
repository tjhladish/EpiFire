#include "Quadtree.h"

Quadtree::Quadtree(QVector<Particle *> &particles) {
    /* Compute bounds. */
    double x1 = std::numeric_limits<double>::max();
    double y1 = x1;
    double x2 = std::numeric_limits<double>::min();
    double y2 = x2;

    for (auto p: particles) {
        if (p->x() < x1) x1 = p->x();
        if (p->y() < y1) y1 = p->y();
        if (p->x() > x2) x2 = p->x();
        if (p->y() > y2) y2 = p->y();
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

/**
 * @ignore Recursively inserts the specified particle <i>p</i> into a
 * descendant of node <i>n</i>. The bounds are defined by [<i>x1</i>,
 * <i>x2</i>] and [<i>y1</i>, <i>y2</i>].
 */
void Quadtree::insertChild( QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2 ) {
    /* Compute the split point, and the quadrant in which to insert p. */
    double sx = (x1 + x2) * .5;
    double sy = (y1 + y2) * .5;
    bool  right = p->x() >= sx;
    bool bottom = p->y() >= sy;

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
void Quadtree::insert( QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2 ) {
    //std::cerr << "insert: " << n << " " << p->x <<"," << p->y << std::endl;
    if (n->leaf) {
        if (n->p) {
            /*
             * If the particle at this leaf node is at the same position as the new
             * particle we are adding, we leave the particle associated with the
             * internal node while adding the new particle to a child node. This
             * avoids infinite recursion.
             */
            if ((fabs(n->p->x() - p->x()) + fabs(n->p->y() - p->y())) < .01) {
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
