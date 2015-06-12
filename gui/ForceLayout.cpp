#include "ForceLayout.h"

using namespace std;

ForceLayout::ForceLayout(): dragConstant(.1),      // 0.1
                           chargeConstant(-40),    // -40
                           chargeMinDistance(2),   //   2
                           chargeMaxDistance(100), // 500
                           chargeTheta(.9),
                           springConstant(.1),
                           springDamping(.3),
                           springLength(20) {}


void ForceLayout::doLayout(std::vector<Particle*>& particles, int iterations=1) {
    double xmin = -150;
    double ymin = -150;
    double xmax = 150;
    double ymax = 150;
    //randomLayout();
    // kl used for spring forces
    std::vector<double> kl;
    for (auto p: particles) {
        for (auto link: p->linksOut) {
            Particle* n = link->dest;
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
        //for (c = this.constraints; c; c = c.next) {c.apply(this.particles, q);
        //    sim.constraint(pv.Constraint.bound().x(6, w - 6).y(6, h - 6));
        //}

        // clear old forces
        for (auto p: particles) {
            p->x = p->x < xmin ? xmin : (p->x > xmax ? xmax : p->x);
            p->y = p->y < ymin ? ymin : (p->y > ymax ? ymax : p->y);
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
            std::vector<Link*> links_out = p->linksOut;
            for (unsigned int i = 0; i < links_out.size(); ++i) {
                Particle* a = links_out[i]->source;
                Particle* b = links_out[i]->dest;
                double dx = a->x - b->x;
                double dy = a->y - b->y;
                double dn = sqrt(dx * dx + dy * dy);
                double dd = dn ? (1.0 / dn) : 1.0;
                double ks = springConstant * kl[i]; // normalized tension
                double kd = springDamping * kl[i]; // normalized damping
                double kk = (ks * (dn - springLength) + kd * (dx * (a->vx - b->vx) + dy * (a->vy - b->vy)) * dd) * dd;
                double fx = -kk * (dn ? dx : (0.01 * (0.5 - rand()/RAND_MAX)));
                double fy = -kk * (dn ? dy : (0.01 * (0.5 - rand()/RAND_MAX)));
                a->fx += fx;
                a->fy += fy;
                b->fx -= fx;
                b->fy -= fy;
            }
        }

        /* Position Verlet integration. */
        for ( auto p: particles) {
            double px = p->px;
            double py = p->py;
            p->px = p->x;
            p->py = p->y;
           // cerr << "t, v, px, f: " << t << " " << p->vx << " " << px << " " << p->fx << endl;
            p->x += p->vx = ((p->x - px) + p->fx);
            p->y += p->vy = ((p->y - py) + p->fy);
        }

        delete q;   //delete quad tree
    }
}
