#include "ForceLayout.h"

using namespace std;

ForceLayout::ForceLayout(): dragConstant(0.1),     // 0.1
                           chargeConstant(-10),    // -40
                           chargeMinDistance(2),   //   2
                           chargeMaxDistance(50),  // 500
                           chargeTheta(.9),        // 0.9
                           springConstant(.025),     // 0.1
                           springDamping(.075),      // 0.3
                           springLength(20),
                           springMaxForce(10),
                           speedMax(20),
                           xmin(-150),
                           xmax(150),
                           ymin(-150),
                           ymax(150){}


void ForceLayout::doLayout(QVector<Particle *> &particles, int iterations=1) {
    // cerr << "doLayout()\n";
    // kl used for spring forces
    std::vector<double> kl;
    for (auto p: particles) {
        for (auto link: p->edgesOut()) {
            Particle* n = link->dest();
            //kl.push_back( 1.0/pow(std::max(p->totalDegree(), n->totalDegree()), 0.25) );
            const float d1 = p->totalDegree();
            const float d2 = n->totalDegree();
            kl.push_back( 1.0/pow(sqrt((d1*d1 + d2*d2)/2.0), 0.5) );
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
            const double cx = p->x();
            const double cy = p->y();
            double nx = cx;
            double ny = cy;
            if (cx < xmin) p->px = nx = xmin;
            else if (cx > xmax) p->px = nx = xmax;
            if (cy < ymin) p->py = ny = ymin;
            else if (cy > ymax) p->py = ny = ymax;
            if (nx != cx or ny != cy) p->setPos(nx,ny);

            // clear old forces
            p->fx = 0;
            p->fy = 0;
        }

        // drag
        if (dragConstant) {
            for (auto p: particles) {
                p->fx -= dragConstant * p->vx;
                p->fy -= dragConstant * p->vy;
            }
        }

        // charge
        _accumulateCharge(q->root);
        for (auto p: particles) {
            _chargeforces(q->root, p, q->xMin, q->yMin, q->xMax, q->yMax);
        }

        // spring
        for (auto p: particles) {
            QVector<Link*> links_out = p->edgesOut();
            for (int i = 0; i < links_out.size(); ++i) {
                Particle* a = links_out[i]->source();
                Particle* b = links_out[i]->dest();
                const double dx = a->x() - b->x();         // x displacement
                const double dy = a->y() - b->y();         // y displacement
                const double dn = sqrt(dx * dx + dy * dy); // spring length
                const double dd = dn ? (1.0 / dn) : 1.0;   // 1 / spring length
                const double ks = springConstant * kl[i];  // normalized tension
                const double kd = springDamping * kl[i];   // normalized damping
                const double kk = (ks * (dn - springLength) + kd * (dx * (a->vx - b->vx) + dy * (a->vy - b->vy)) * dd) * dd;
                double fx = -kk * (dn ? dx : (0.001 * (0.5 - rand()/RAND_MAX)));
                double fy = -kk * (dn ? dy : (0.001 * (0.5 - rand()/RAND_MAX)));
                const double fn = sqrt(fx*fx + fy*fy);
                const double governor = fn > springMaxForce ? fn/springMaxForce: 1.0;
                fx /= governor;
                fy /= governor;
                a->fx += fx;
                a->fy += fy;
                b->fx -= fx;
                b->fy -= fy;
            }
        }

        /* Position Verlet integration. */
        for ( auto p: particles) {
            const double px = p->px;
            const double py = p->py;
            const double cx = p->x();
            const double cy = p->y();
            p->px = cx;
            p->py = cy;
            p->vx = cx - px + p->fx;
            p->vy = cy - py + p->fy;
            const double s = _speed(p);
            const double governor = s > speedMax ? s/speedMax : 1.0;
            const double x = cx + p->vx/governor;
            const double y = cy + p->vy/governor;
            p->setPos(x,y);
        }

        delete q;   //delete quad tree
    }
}

double ForceLayout::_speed(Particle *n) { return sqrt(n->vx * n->vx + n->vy * n->vy); }

void ForceLayout::_chargeforces(QuadtreeNode* n, Particle *p, double x1, double y1, double x2, double y2) {
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
