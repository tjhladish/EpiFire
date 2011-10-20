#include <Percolation_Sim.h>

// This example simulates an epidemic on a changing network using an SIR
// percolation simulation.  Since networks that change with time can be
// implemented in many different ways, here is a more detailed description:
//
// During each infectious period, 10% of all nodes "migrate".  A different
// 10% is randomly selected each period.  If a nodes is in the migrating 
// group, all of its edges are broken; this means the node's neighbors also
// end up with at least one broken edge.  These broken edges ("stubs") are
// then randomly reconnected.  The reconnection process will make sure that
// no self-loops or parallel edges get created (it's possible that
// additional edges will get shuffled in order to get rid of these).


int main() {

    // Construct an undirected network
    // with size 10,000 and a poisson(5)
    // degree distribution
    Network net("name", Network::Undirected);
    net.populate(10000);
    net.fast_random_graph(5);

    // Create variables to handle node migration
    vector<Node*> nodes = net.get_nodes();
    vector<Edge*> broken_edges;
    double fraction_to_migrate = 0;
    int num_to_migrate = fraction_to_migrate * net.size();

    // Set up simulation
    Percolation_Sim sim(&net);
    sim.set_transmissibility(0.25); // Epidemic size will be 37-38%
    sim.rand_infect(10); // The probability of an epidemic will be ~99%

    // Continue the simulation as long as someone is still infected
    while (sim.count_infected() > 0) {
        sim.step_simulation(); // This advances the simulation by one infectious pd
        
        shuffle(nodes, net.get_rng()); // Shuffle the nodes
        for (int i = 0; i < num_to_migrate; i++) { // Migrate the first 1000
            Node* node = nodes[i];
            vector<Edge*> edges_out = node->get_edges_out();
            vector<Edge*> edges_in = node->get_edges_in();
            
            // Break all outbound edges
            for (unsigned int j = 0; j < edges_out.size(); j++) {
                Edge* outbound_edge = edges_out[j];
                if (not outbound_edge->is_stub() ) {
                    edges_out[j]->break_end();
                    broken_edges.push_back(edges_out[j]);
                }
            }
            
            // Break all inbound edges
            for (unsigned int j = 0; j < edges_in.size(); j++) {
                Edge* inbound_edge = edges_in[j];
                if (not inbound_edge->is_stub() ) {
                    edges_in[j]->break_end();
                    broken_edges.push_back(edges_in[j]);
                }
            }
        }

        // Randomly reconnect all broken edges
        net.rand_connect_stubs(broken_edges);
        broken_edges.clear();
    }
    // The following line is not necessary, but validate() will check
    // to make sure that the network structure is still valid.  For example,
    // If Node A has an edge leading to Node B, Node B must know it has an
    // inbound edge from Node A.  In undirected graphs, there must be a 
    // complementary edge from Node B to Node A.  (Technically, these are arcs.) 
    net.validate();

    // Print out the final epidemic size
    cout << sim.epidemic_size() << endl;
    return 0;
}
