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
            cx += c->cn * c->cx;
        }

    public:
        ForceLayout(): dragConstant(.1), 
                                   chargeConstant(-40), 
                                   chargeMinDistance(2),
                                   chargeMaxDistance(500), 
                                   chargeTheta(.9), 
                                   springConstant(.1),
                                   springDamping(.3), 
                                   springLength(20) {}


        void randomLayout() {
            /*
            for (var i = 0, n; i < nodes.length; i++) {
                n = nodes[i];
                if (isNaN(n.x)) n.x = w / 2 + 40 * Math.random() - 20;
                if (isNaN(n.y)) n.y = h / 2 + 40 * Math.random() - 20;
            }
            */
        }

        void doLayout(std::vector<Particle*>& particles) {
            randomLayout();
            // kl used for spring forces
            std::vector<double> kl;
            for (auto p: particles) {
                for (auto link: p->linksOut) {
                    Particle* n = link->dest;
                    kl.push_back( 1.0/sqrt(std::max(p->totalDegree(), n->totalDegree())) );
                }
            }

            for (int t = 0; t<100; ++t) {
                /*
                 * Assumptions:
                 * - The mass (m) of every particles is 1.
                 * - The time step (dt) is 1.
                 */

                /* Position Verlet integration. */
                for ( auto p: particles) {
                    double px = p->px;
                    double py = p->py;
                    p->px = p->x;
                    p->py = p->y;
                    p->x += p->vx = ((p->x - px) + p->fx);
                    p->y += p->vy = ((p->y - py) + p->fy);
                }

                /* Apply constraints, then accumulate new forces. */
                Quadtree* q = new Quadtree(particles);
                //for (c = this.constraints; c; c = c.next) {c.apply(this.particles, q);
                //    sim.constraint(pv.Constraint.bound().x(6, w - 6).y(6, h - 6));
                //}
                // clear old forces
                for (auto p: particles) {
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

                delete q;   //delete quad tree
            }
        }
};
