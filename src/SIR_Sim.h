#ifndef SIR_H
#define SIR_H

#include <cstdlib>
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_errno.h>
#include <vector>
#include <cmath>
#include <iostream>

#define NBINS 3

using namespace std;

class SIR {

    private:
        const double R0;
        const double beta;
        const double gamma;
        double* y;

    public:
        SIR() : R0(0.0), beta(0.0), gamma(0.0) {}
        SIR(double b, double g): R0(b/g), beta(b), gamma(g) {}

        ~SIR() {};

        void initialize( double S, double I, double R) {
            y = new double[NBINS];
            y[0] = S; y[1] = I; y[2] = R;
        }

        void derivative(double const y[], double dydt[], double t) {
            dydt[0] = -beta*y[0]*y[1];
            dydt[1] = +beta*y[0]*y[1] - gamma*y[1];
            dydt[2] = +gamma*y[1];
        }


        static int function(double t, double const y[], double dydt[], void *params) {
            struct Params { SIR* model; };
            Params* par = static_cast <Params*> (params);
            par->model->derivative(y, dydt, t);
            return GSL_SUCCESS;
        }

        
        int run_simulation() {
            struct Params { SIR* model;};
            Params par;
            par.model = this;

            gsl_odeiv_step*     s = gsl_odeiv_step_alloc(gsl_odeiv_step_rkf45, NBINS);
            gsl_odeiv_control*  c = gsl_odeiv_control_y_new(1e-20, 0);
            gsl_odeiv_evolve*   e = gsl_odeiv_evolve_alloc(NBINS);

            gsl_odeiv_system sys = {SIR::function, NULL, NBINS, &par};

            double t  = 0.0;      //initial time 
            double dt = 1e-6;     //time step
            int tFrame=2000;

            int steps=0;
            while (t < tFrame) {  //convergence check here
                int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &t, tFrame, &dt, y);
                if (status != GSL_SUCCESS) { return status; }
            }
            std::cout << y[0] << " " << y[1] << " " << y[2] << endl;;
            return 0;
        }

};

#endif /* SIR_H_ */
