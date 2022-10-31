#include "Gillespie_MassAction_Sim.h"

int main() { 

    int N        = 1000000;
    double BETA  = 1/(double) 4;
    double GAMMA = 1/(double) 6;

    for(int i=0; i<50; i++ ) {
        Gillespie_MassAction_Sim sim(N,GAMMA,BETA);
        std::random_device rand;
        sim.rng.seed(rand()); //seed RNG for simulator
        sim.rand_infect(1);
        sim.run_simulation();
        cout << sim.epidemic_size() << endl;
    }

    return 0;
}


