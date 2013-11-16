#include "Gillespie_Network_SEIRS_Sim.h"

int main() { 

    Network net = Network("gillespie toy", Network::Undirected);
    net.populate(10000);
    net.fast_random_graph(8);

    double mu    = 1.0/2.0; // E -> I transition rate
    double beta  = 1.0/3.0; // contact rate
    double gamma = 1.0/6.0; // I -> R transition rate
    double immunity_duration = 365;

    Gillespie_Network_SEIRS_Sim sim(&net, mu, beta, gamma, immunity_duration);
    //sim.rand_infect(1);
    sim.print_state_counts();

    for(int i=0; i<1000; i++ ) {
        if (i % 365 == 0) {
            cout << "Introducing new infections\n";
            sim.rand_infect(5);
        }
        if (sim.run_simulation(1) == 0) {
            sim.print_state_counts();
            break;
        }
        sim.print_state_counts();
    }

    return 0;
}


