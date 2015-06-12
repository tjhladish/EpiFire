#include "ForceLayout.h"

using namespace std;

ForceLayout::ForceLayout(): dragConstant(0.1),     // 0.1
                           chargeConstant(-40),    // -40
                           chargeMinDistance(2),   //   2
                           chargeMaxDistance(100), // 500
                           chargeTheta(.9),        // 0.9
                           springConstant(.01),     // 0.1
                           springDamping(.3),      // 0.3
                           springLength(20),
                           xmin(-150),
                           xmax(150),
                           ymin(-150),
                           ymax(150){}


void ForceLayout::doLayout(QVector<GNode*>& particles, int iterations=1) {
    // kl used for spring forces
    std::vector<double> kl;
    for (auto p: particles) {
        for (auto link: p->edgesOut()) {
            GNode* n = link->dest();
            kl.push_back( 1.0/sqrt(std::max(p->totalDegree(), n->totalDegree())) );
        }
    }

    for (int t = 0; t<iterations; ++t) {
     /*
     * Assumptions:
     * - The mass (m) of every particles is 1.
     * - The time step (dt) is 1.
     */

        /* Apply constraints, then accumulate new forces. */
        Quadtree* q = new Quadtree(particles);

        for (auto p: particles) {
            // make particles stay in scene
            const double x = p->x() < xmin ? xmin : (p->x() > xmax ? xmax : p->x());
            cerr << "x, xi, min, max: " << x << " " << p->x() << " " << xmin << " " << xmax << endl;
            const double y = p->y() < ymin ? ymin : (p->y() > ymax ? ymax : p->y());
            p->setPos(x,y);

            // clear old forces
            p->fx = 0;
            p->fy = 0;
        }

        //double v_sum = 0;
        // drag
        if (dragConstant) {
            for (auto p: particles) {
                p->fx -= dragConstant * p->vx;
                p->fy -= dragConstant * p->vy;
                //v_sum += sqrt(p->vx*p->vx + p->vy*p->vy);
            }
        }
        //std::cerr << v_sum << std::endl;
        // charge
        _accumulateCharge(q->root);
        for (auto p: particles) {
            _chargeforces(q->root, p, q->xMin, q->yMin, q->xMax, q->yMax);
        }

        // spring
        for (auto p: particles) {
            QVector<GEdge*> links_out = p->edgesOut();
            for (int i = 0; i < links_out.size(); ++i) {
                GNode* a = links_out[i]->source();
                GNode* b = links_out[i]->dest();
                double dx = a->x() - b->x();
                double dy = a->y() - b->y();
                double dn = sqrt(dx * dx + dy * dy);
                double dd = dn ? (1.0 / dn) : 1.0;
                double ks = springConstant * kl[i]; // normalized tension
                double kd = springDamping * kl[i]; // normalized damping
                double kk = (ks * (dn - springLength) + kd * (dx * (a->vx - b->vx) + dy * (a->vy - b->vy)) * dd) * dd;
                double fx = -kk * (dn ? dx : (0.01 * (0.5 - rand()/RAND_MAX)));
                double fy = -kk * (dn ? dy : (0.01 * (0.5 - rand()/RAND_MAX)));
if (i == 0) cerr << dx << " " << dy << " " << dn << " " << dd << " | " << ks << " " << kd << " " << kk << " | " << fx << " " << fy << endl;
                a->fx += fx;
                a->fy += fy;
                b->fx -= fx;
                b->fy -= fy;
            }
        }

        /* Position Verlet integration. */
        for ( auto p: particles) {
/*          double px = p->px;
            double py = p->py;
            p->px = p->x;
            p->py = p->y;
           // cerr << "t, v, px, f: " << t << " " << p->vx << " " << px << " " << p->fx << endl;
            p->x += p->vx = ((p->x - px) + p->fx);
            p->y += p->vy = ((p->y - py) + p->fy);*/

            const double px = p->px;
            const double py = p->py;
            const double cx = p->x();
            const double cy = p->y();
            p->px = cx;
            p->py = cy;
            p->vx = cx - px + p->fx;
            p->vy = cy - py + p->fy;
            const double x = cx + p->vx;
            const double y = cy + p->vy;
            p->setPos(x,y);
        }

        delete q;   //delete quad tree
    }
}

double ForceLayout::_speed(GNode* n) { return n->vx * n->vx + n->vy * n->vy; }

void ForceLayout::_chargeforces(QuadtreeNode* n, GNode* p, double x1, double y1, double x2, double y2) {
    double dx = n->cx - p->x();
    double dy = n->cy - p->y();
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
void ForceLayout::_accumulateCharge(QuadtreeNode* n) {
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
        cx += chargeConstant * n->p->x();
        cy += chargeConstant * n->p->y();
    }
    n->cx = cx / n->cn;
    n->cy = cy / n->cn;
}

void ForceLayout::_accumulateChild(QuadtreeNode* c, QuadtreeNode* n, double& cx, double& cy) {
    _accumulateCharge(c);
    n->cn += c->cn;
    cx += c->cn * c->cx;
    cy += c->cn * c->cy;
}
