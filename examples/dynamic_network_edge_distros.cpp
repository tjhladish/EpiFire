#include "Gillespie_Network_SEIRS_Sim.h"
#include <fstream>

int main(int argc, char* argv[]) {

    Network net = Network("scanning toy", Network::Undirected);
    net.populate(209264);

    ifstream source(argv[1]);

    while(net.add_edgelist(source)) {
    	vector< int > comps = net.get_deg_dist();
   		for (int i=1; i<comps.size(); i++) {

   		    cout << comps[i] << " ";

   		}
   		cout << endl;

    	net.clear_edges();
    }

    return 0;
}


