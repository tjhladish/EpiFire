#ifndef FORCELAYOUT_H
#define FORCELAYOUT_H
#include "Quadtree.h"
#include <random>
#include <vector>
#include <iostream>
#include "node.h"
#include <QVector>

class GNode;
class GEdge;
class QuadtreeNode;
//class Particle;
//class Link;
typedef GNode Particle;
typedef GEdge Link;

class ForceLayout {
    private:
        double dragConstant;
        double chargeConstant;
        double chargeMinDistance;
        double chargeMaxDistance;
        double chargeTheta;
        double springConstant;
        double springDamping;
        double springLength;
        double springMaxForce;
        double speedMax;
        double xmin;
        double xmax;
        double ymin;
        double ymax;

        double _speed(Particle* n);
        void _chargeforces(QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2);
        void _accumulateCharge(QuadtreeNode* n);
        void _accumulateChild(QuadtreeNode* c, QuadtreeNode* n, double& cx, double& cy);

    public:
        ForceLayout();

        void doLayout(QVector<Particle*> &particles, int iterations);
        void set_dimensions(double x, double X, double y, double Y) {xmin=x; xmax=X; ymin=y; ymax=Y;}
};

#endif
