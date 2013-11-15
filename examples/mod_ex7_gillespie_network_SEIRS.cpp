#include "Gillespie_Network_SEIRS_Sim.h"
#include <fstream>

int main(int argc, char* argv[]) {

    Network net = Network("scanning toy", Network::Undirected);
    net.populate(209264);

    string filename = "/Users/carlpearson/Dropbox/epidemics4share/day1els.o";

    cout << filename << endl;

    ifstream source(filename.c_str());

//    double mu    = 1.0/2.0; // E -> I transition rate
//    double beta  = 1.0/3.0; // contact rate
//    double gamma = 1.0/6.0; // I -> R transition rate
//    double immunity_duration = 365;

    while(net.add_edgelist(source)) {

    	vector<vector<Node*> > comps = net.get_components();
   		for (int i=0;i<comps.size();i++) {
   		    if (comps[i].size() > 1) {
   		    	cout << comps[i].size() << " ";
   		    }
   		}
   		cout << endl;

    	net.clear_edges();

//        Gillespie_Network_SEIRS_Sim sim(&net, mu, beta, gamma, immunity_duration);
//        sim.rand_infect(1);
//        sim.run_simulation(1000);
        //cout << sim.current_epidemic_size() << endl;
    }

    return 0;
}


