#include "Event_Driven_SEIRS_Sim.h"

int main() {

    //net.populate(1903210);
    //net.fast_random_graph(8);

    //int N = 1903210;
    int N = 2e7;
    double mu    = 1.0/4.0; // E -> I transition rate
    double beta  = 3.0/8.0; // contact rate
    double gamma = 1.0/8.0; // I -> R transition rate
    double immunity_duration = 10000;
    double social_distancting_effect = 0.73;
    int social_distancting_threshold = 258;

    for(int i=0; i<1; i++ ) {
        Event_Driven_SEIRS_Sim sim(N, mu, beta, gamma, immunity_duration, social_distancting_effect, social_distancting_threshold);
        sim.rng.seed(time(0)); // this simulator has its own RNG which must be seeded as well
        sim.rand_infect(100);
        sim.run_simulation(365, i);
        //cout << sim.current_epidemic_size() << endl;
    }

    return 0;
}


