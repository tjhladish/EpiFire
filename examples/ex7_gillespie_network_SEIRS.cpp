#include "Gillespie_Network_SEIRS_Sim.h"

int main(int argc, char* argv[]) {

	Network net = Network("scanning toy", Network::Undirected);
	net.populate(209264);

	const int sourceStep = atoi(argv[2]);
	//sscanf (argv[2],"%d",&sourceStep);

	// TODO parse these from args?
    const double mu    = 1.0/1.2; // E -> I transition rate
    const double beta  = 1.0/0.5; // contact rate
    const double gamma = 1.0/4.1; // I -> R transition rate
    const double immunity_duration = 365;

    const int externalInfectionRate = 10;
    const int externalInfectionSites = 1;

    Gillespie_Network_SEIRS_Sim sim(&net, mu, beta, gamma, immunity_duration);
    //sim.rand_infect(1);

    for (int rep=0; rep<1000; rep++ ) {
        ifstream source(argv[1]);

        sim.print_state_counts();
        for(int i=0; i<2023; i++ ) {
            if (i % sourceStep == 0) {
                //cout << "Swapping edges\n";
                net.clear_edges();
                net.add_edgelist(source);
            }
            if (i % externalInfectionRate == 0) {
                //cout << "Introducing new infections\n";
                sim.rand_infect(externalInfectionSites);
            }
            if (sim.run_simulation(1) == 0) {
                sim.print_state_counts();
                break;
            }

            sim.print_state_counts();
        }
        sim.reset();
    }

    return 0;
}

