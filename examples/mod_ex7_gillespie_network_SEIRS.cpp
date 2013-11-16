#include "Gillespie_Network_SEIRS_Sim.h"
#include <fstream>

int main(int argc, char* argv[]) {

    Network net = Network("scanning toy", Network::Undirected);
    net.populate(209264);

    string filename = "/Users/carlpearson/Dropbox/epidemics4share/day1els.o";

    cout << filename << endl;

    ifstream source(filename.c_str());

    while(net.add_edgelist(source)) {

    	vector<vector<Node*> > comps = net.get_components();
   		for (int i=0;i<comps.size();i++) {
   		    if (comps[i].size() > 1) {
   		    	cout << comps[i].size() << " ";
   		    }
   		}
   		cout << endl;

    	net.clear_edges();
    }

    return 0;
}


