#include "Gillespie_MassAction_Sim.h"

int main() { 

    int N     = 1000000;
    double GAMMA = 1/(double) 6;
    double BETA  = 1/(double) 4;

    for(int i=0; i<50; i++ ) {
        Gillespie_MassAction_Sim sim(N,GAMMA,BETA);
        sim.rand_infect(100);
        sim.run_simulation();
        cout << sim.epidemic_size() << endl;
    }

    return 0;
}


