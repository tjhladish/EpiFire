#include "Gillespie_Network_SEIRS_Sim.h"

int main() { 

    Network net = Network("gillespie toy", Network::Undirected);
    net.populate(10000);
    net.fast_random_graph(8);

    double mu    = 1.0/2.0; // E -> I transition rate
    double beta  = 1.0/3.0; // contact rate
    double gamma = 1.0/6.0; // I -> R transition rate
    double immunity_duration = 365;

    for(int i=0; i<1; i++ ) {
        Gillespie_Network_SEIRS_Sim sim(&net, mu, beta, gamma, immunity_duration);
        sim.rand_infect(1);
        sim.run_simulation(1000);
        //cout << sim.current_epidemic_size() << endl;
    }

    return 0;
}


