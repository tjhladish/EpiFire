#include "Deterministic_Network_SIR_Sim.h"

int main() { 

    //int N        = 1000000;
    //double R0    = 1.5;
    double R  = 2;
    double MU = 0.2;
    vector<double> degree_dist;
    degree_dist.push_back(0.0);
    degree_dist.push_back(2.0);
    degree_dist.push_back(4.0);
    degree_dist.push_back(3.0);
    degree_dist.push_back(1.0);

    double theta = 1.0;
    double pI = 1.0/1000;
    double pS = 1.0 - pI;
    double I = pI;

    Deterministic_Network_SIR_Sim sim(R, MU, degree_dist);
    sim.initialize(theta, pS, pI, I);
    while (sim.y[3] > 0.1*I) {
        sim.printY();
        sim.step_simulation(1);
    }
    sim.printY();

    //sim.run_simulation();

    return 0;
}


