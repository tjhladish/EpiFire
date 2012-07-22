#ifndef DETERMINISTIC_NETWORK_SIR_H
#define DETERMINISTIC_NETWORK_SIR_H

#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_errno.h>
#include <iostream>
#include <math.h>
#include "DiffEq_Sim.h"

using namespace std;

class Deterministic_Network_SIR_Sim : public DiffEq_Sim {

    private:
        const double r;
        const double mu;
        const vector<double> deg_dist;

    public:
        Deterministic_Network_SIR_Sim() : r(0.0), mu(0.0) { nbins=4;}
        Deterministic_Network_SIR_Sim(double r_param, double mu_param, vector<double> deg_dist_param): r(r_param), mu(mu_param), deg_dist(deg_dist_param) { nbins=4; }
        ~Deterministic_Network_SIR_Sim() {};

        void initialize( double theta, double pS, double pI, double I) {
            y = new double[nbins];
            y[0] = theta; 
            y[1] = pS; 
            y[2] = pI;
            y[3] = I;
        }

        double current_susceptible() { return g( y[0] ); }
        double current_infectious() { return y[3]; }
        double current_recovered() { return 1.0 - current_susceptible() - current_infectious(); }

        double g( double theta) {
            double val = 0;
            for (unsigned int i = 0; i<deg_dist.size(); i++) {
                val += deg_dist[i] * pow(theta,i);
            }
            return val;
        }

        double dg( double theta) {
            double val = 0;
            for (unsigned int i = 1; i<deg_dist.size(); i++) {
                val += (i) * deg_dist[i] * pow(theta,i-1);
            }
            return val;
        }

        double ddg( double theta) {
            double val = 0;
            for (unsigned int i = 2; i<deg_dist.size(); i++) {
                val += (i) * (i-1) *deg_dist[i] * pow(theta,i-2);
            }
            return val;
        }
/*
        g(x) = p_0 + p_1 * x + p_2 *x^2

        double g(double theta)  { return 0*pow(theta,0) + 0*pow(theta,1) + .25*pow(theta,2) + .5*pow(theta,3)  + .25*pow(theta,4); }
        double dg(double theta) { return                1*0*pow(theta,0) + .5*pow(theta,1)  + 1.5*pow(theta,2) + 1*pow(theta,3); }
        double ddg(double theta){ return                                   .5*pow(theta,0)  + 3*pow(thetha,1)  + 3*pow(theta,2); }
*/
        void derivative(double const y[], double dydt[]) {
            //dydt[0] = -beta*y[0]*y[1];
            //dydt[1] = +beta*y[0]*y[1] - gamma*y[1];
            //dydt[2] = +gamma*y[1];

            const double theta = y[0];
            const double pS = y[1];
            const double pI = y[2];
            const double I = y[3];

            dydt[0] = -r * pI * theta;                                                  // dtheta.dt
            dydt[1] = r *  pS * pI * (1 - theta * ddg(theta)/dg(theta));                // dpS.dt
            dydt[2] = r *  pI * pS * theta * ddg(theta)/dg(theta) - pI*(1-pI)*r- pI*mu; // dpI.dt

            dydt[3] = r * pI * theta * dg(theta) - mu*I;
        }

};

/*
library("odesolve")

param=c(0.2,0.2)       # r, mu
initvalues=c(1, 0.999, 0.001); # theta, pS, pI
times=seq(1,200,1);

g   = function(theta) { .25*theta^2 + .5*theta^3 + .25*theta^4; }
dg  = function(theta) { .5*theta^1 + 1.5*theta^2 + 1*theta^3; }
ddg = function(theta) { .5*theta^0 + 3*theta^1  + 3*theta^2; }

logGrowth <- function(t, y, p) {
     r=p[1]; mu=p[2];
     theta=y[1]; pS=y[2]; pI=y[3];
     
     dtheta.dt <- -r * pI * theta;
     dpS.dt    <- r *  pS * pI * (1 - theta * ddg(theta)/dg(theta));
     dpI.dt    <- r *  pI * pS * theta * ddg(theta)/dg(theta) - pI*(1-pI)*r- pI*mu;

     print(dtheta.dt)
     #print(c(r,theta,dtheta.dt,dpS.dt,dpI.dt));
     #print(c(dtheta.dt,dpS.dt,dpI.dt))

     return(list(c(dtheta.dt,dpS.dt,dpI.dt)))
}

I <- lsoda(initvalues,times,logGrowth,param);
*/

#endif
