#include <Simulator.h>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>

#define N_NODES 1000 
int main() {
    MTRand mtrand = MTRand();
    for (int i = 0; i < 5; i++ ) {
        Network net("test", Network::Undirected );
        net.populate(N_NODES);
        net.rand_connect_poisson(10);
	    cout << "Network size: " << net.size()     << endl;
        cout << "Mean degree: "  << net.mean_deg() << endl;
        cout << "Transitivity: " << net.transitivity() << endl;

        vector<Node*> comp = net.get_biggest_component();
        cout << "Size of largest component: " << comp.size() << endl;

        // Write network out as graphviz file
        stringstream ss;
        ss << "test" << i << ".dot";
        net.graphviz(ss.str());
    }
return 0;
}

