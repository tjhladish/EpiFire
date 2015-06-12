#include "Quadtree.h"
#include <random>
#include <vector>
#include <iostream>

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
        double xmin;
        double xmax;
        double ymin;
        double ymax;

        double _speed(Particle* n) { return n->vx * n->vx + n->vy * n->vy; }

        void _chargeforces(QuadtreeNode* n, Particle* p, double x1, double y1, double x2, double y2) {
            double dx = n->cx - p->x;
            double dy = n->cy - p->y;
            double dn = 1.0 / sqrt(dx * dx + dy * dy);
            const double min1 = 1.0/chargeMinDistance;
            const double max1 = 1.0/chargeMaxDistance;

            /* Barnes-Hut criterion-> */
            if ((n->leaf && (n->p != p)) || ((x2 - x1) * dn < chargeTheta)) {
                if (dn < max1) return;
                if (dn > min1) dn = min1;
                double kc = n->cn * dn * dn * dn;
                double fx = dx * kc;
                double fy = dy * kc;
                p->fx += fx;
                p->fy += fy;
            } else if (!n->leaf) {
                double sx = (x1 + x2) * .5;
                double sy = (y1 + y2) * .5;
                if (n->c1) _chargeforces(n->c1, p, x1, y1, sx, sy);
                if (n->c2) _chargeforces(n->c2, p, sx, y1, x2, sy);
                if (n->c3) _chargeforces(n->c3, p, x1, sy, sx, y2);
                if (n->c4) _chargeforces(n->c4, p, sx, sy, x2, y2);
                if (dn < max1) return;
                if (dn > min1) dn = min1;
                if (n->p && (n->p != p)) {
                    double kc = chargeConstant * dn * dn * dn;
                    double fx = dx * kc;
                    double fy = dy * kc;
                    p->fx += fx;
                    p->fy += fy;
                }
            }
        }
        void _accumulateCharge(QuadtreeNode* n) {
            double cx = 0;
            double cy = 0;
            n->cn = 0;

            if (!n->leaf) {
                if (n->c1) _accumulateChild(n->c1, n, cx, cy);
                if (n->c2) _accumulateChild(n->c2, n, cx, cy);
                if (n->c3) _accumulateChild(n->c3, n, cx, cy);
                if (n->c4) _accumulateChild(n->c4, n, cx, cy);
            }
            if (n->p) {
                n->cn += chargeConstant;
                cx += chargeConstant * n->p->x;
                cy += chargeConstant * n->p->y;
            }
            n->cx = cx / n->cn;
            n->cy = cy / n->cn;
        }

        void _accumulateChild(QuadtreeNode* c, QuadtreeNode* n, double& cx, double& cy) {
            _accumulateCharge(c);
            n->cn += c->cn;
            cx += c->cn * c->cx;
            cy += c->cn * c->cy;
        }

    public:
        ForceLayout();

        void randomLayout() {
            /*
            for (var i = 0, n; i < nodes.length; i++) {
                n = nodes[i];
                if (isNaN(n.x)) n.x = w / 2 + 40 * Math.random() - 20;
                if (isNaN(n.y)) n.y = h / 2 + 40 * Math.random() - 20;
            }
            */
        }

        void doLayout(std::vector<Particle*>& particles, int iterations);
        void set_dimensions(double x, double X, double y, double Y) {xmin=x; xmax=X; ymin=y; ymax=Y;}
};
