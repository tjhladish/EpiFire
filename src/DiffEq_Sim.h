#ifndef DIFFEQ_SIM_H
#define DIFFEQ_SIM_H

#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_errno.h>
#include <iostream>

using namespace std;


class DiffEq_Sim {

    public:
        DiffEq_Sim() {};
        ~DiffEq_Sim() {};

        int nbins;
        double* y;

        void printY() { for(int i=0; i < nbins; i++) { cout << y[i] << " ";} cout << endl; }

        virtual void initialize() {}
        virtual void derivative(const double y[], double dydt[]){}

        static int function(double t, double const y[], double dydt[], void *params) {
            DiffEq_Sim* model = static_cast <DiffEq_Sim*> (params);
            model->derivative(y, dydt);
            return GSL_SUCCESS;
        }


        int run_simulation() {
            gsl_odeiv_evolve*   e = gsl_odeiv_evolve_alloc(nbins);
            gsl_odeiv_control*  c = gsl_odeiv_control_y_new(1e-20, 0);
            gsl_odeiv_step*     s = gsl_odeiv_step_alloc(gsl_odeiv_step_rkf45, nbins);

            gsl_odeiv_system sys = {function, NULL, nbins, this };

            double t  = 0.0;      //initial time 
            double dt = 1e-6;     //time step
            int tFrame=2000;

            while (t < tFrame) {  //convergence check here
                int status = gsl_odeiv_evolve_apply(e, c, s, &sys, &t, tFrame, &dt, y);
                if (status != GSL_SUCCESS) { return status; }
            }

            printY();
            return 0;
        }

};

#endif
