#ifndef SIR_H
#define SIR_H

#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_errno.h>
#include <iostream>
#include "DiffEq_Sim.h"

using namespace std;

class SIR : public DiffEq_Sim {

    private:
        const double beta;
        const double gamma;

    public:
        SIR() : beta(0.0), gamma(0.0) { nbins=3;}
        SIR(double b, double g): beta(b), gamma(g) { nbins=3; }
        ~SIR() {};

        void initialize( double S, double I, double R) {
            y = new double[nbins];
            y[0] = S; y[1] = I; y[2] = R;
        }

        void derivative(double const y[], double dydt[]) {
            dydt[0] = -beta*y[0]*y[1];
            dydt[1] = +beta*y[0]*y[1] - gamma*y[1];
            dydt[2] = +gamma*y[1];
        }

};

#endif
