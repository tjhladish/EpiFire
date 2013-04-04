#include "SIR_Sim.h"

int main() { 

    //int N        = 1000000;
    double R0    = 1.5;
    double BETA  = R0/6.0;
    double GAMMA = 1.0/6.0;

    double I = 1.0/10000;
    double S = 1.0 - I;
    double R = 0.0;

    SIR sim(BETA, GAMMA);
    sim.initialize(S, I, R);

    while (sim.y[1] > 0.1*I) {
        sim.printY();
        sim.step_simulation(1);
    }
    sim.printY();

    return 0;
}


