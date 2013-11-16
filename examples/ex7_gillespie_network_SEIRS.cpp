#include "Gillespie_Network_SEIRS_Sim.h"

int main(int argc, char* argv[]) {

	Network net = Network("scanning toy", Network::Undirected);
	net.populate(209264);
	// string filename = "/Users/carlpearson/Dropbox/epidemics4share/day1els.o";
	ifstream source(argv[1]);

	int sourceStep;
	sscanf (argv[2],"%d",&sourceStep);

    double mu    = 1.0/2.0; // E -> I transition rate
    double beta  = 1.0/1.5; // contact rate
    double gamma = 1.0/6.0; // I -> R transition rate
    double immunity_duration = 365;

    int externalInfectionRate = 10;
    int externalInfectionSites = 1;

    Gillespie_Network_SEIRS_Sim sim(&net, mu, beta, gamma, immunity_duration);
    //sim.rand_infect(1);
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

    return 0;
}


