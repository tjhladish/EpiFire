// Here we will use a local SEIR simulation class that is derived
// from the Percolation_Sim base class
#include "SEIR_Percolation_Sim.h"

/******************************************************************************
/ A small SEIR simulation with a cute visualization tool
/ 
/ This program demonstrates how a derived simulation class can be created.
/ In this case, we made a copy of the Percolation_Sim.h file in the local
/ directory, and renamed the class (and file) to denote that we are changing
/ the state progression to susceptible-EXPOSED-infectous-recovered.  We
/ added some new methods, modified some existing ones, and deleted the ones
/ that weren't being changed, since those would be inherited from the 
/ Percolation_Sim base class.
*******************************************************************************/

int main() {

    // Create and populate a network
    Network net("name", Network::Undirected);
    net.populate(120); // very small, but this way we can visualize it
    
    // Parameterize degree distribution, a truncated Poisson(5)
    double lambda = 5;
    int min = 0; // min degree
    int max = 9; // max degree
    // generate the normalize vector of probabilities
    vector<double> dist = gen_trunc_poisson(lambda, min, max); 
    // use configuration model to connect up the network
    net.rand_connect_user(dist);

    for (int i = 0; i < 10; i++){
        // Choose and run simulation
        SEIR_Percolation_Sim sim(&net);
        // set probability of transmission between neighbors
        sim.set_transmissibility(0.5);
        // randomly set some people to the 'exposed' state
        sim.rand_expose(10);
        
        cout << "Iteration and node states:\n";
        sim.run_simulation();
        
        // Print the degrees so we can see if there are any
        // interesting patterns
        cout << "Deg: ";
        for (int j =0; j< net.size(); j++) {
            cout << net.get_node(j)->deg();
        } cout << endl;
        cout << "Epidemic size: " << sim.epidemic_size() << "\n\n";
        
        sim.reset();
    }
    return 0;
}
