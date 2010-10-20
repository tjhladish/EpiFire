#include <Simulator.h>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <unistd.h>

#define N_NODES 100000 
int main() {
    MTRand mtrand = MTRand();
    for (int i = 0; i < 20; i++ ) {
        cout << i << endl;
        Network net("test", Network::Undirected );
        net.populate(N_NODES);
	    //cout << net.size() << " ";
        //net.erdos_renyi(4);
        net.rand_connect_poisson(10);
//        sleep(5);
        //cout << net.mean_deg() << endl;
        //cout << net.transitivity(net.get_nodes()) << endl;
        //vector<Node*> comp = net.get_major_component();
        //cout << comp.size() << endl;
        //net.graphviz("");
/*	    
	    vector<Node*> my_nodes = net.get_nodes();
	    my_nodes[1] = my_nodes[2];
	    vector<Node*> your_nodes = net.get_nodes();
	    if (my_nodes[1] == your_nodes[2]) cerr << "Yikes!!\n";
	   
	    for (int i = 0; i < N_NODES; i++) {
	        net.get_node(i)->mean_min_path();
	        cout << i << " mean dist: " << setprecision(10) << net.get_node(i)->mean_min_path() << endl;
	        vector<int> distances = net.get_node(i)->min_paths();        
	        cout << i << " to 1 " << distances[1] << endl;
	    }*/
    }
return 0;
}

