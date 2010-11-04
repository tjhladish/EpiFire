#include <ChainBinomial_Sim.h>

int main() {
    // Construct Network
    Network net("name", Network::Undirected);
    net.populate(10000);

    // Let's connect the network using an arbitrary degree distribution
    // Out of 10 nodes, none will have degrees 0 or 1, 1 will have degree 2
    // 3 with degree 3, 2 with degree 4, 1 with degree 5, and 3 with degree 3
    vector<double> degree_dist;
    double tmp_array[] = {0, 0, 1, 3, 2, 1, 3};
    degree_dist.assign(tmp_array,tmp_array+7); // 7 == the length of tmp_array

    // Now make those probabilities that sum to 1
    degree_dist = normalize_dist(degree_dist, sum(degree_dist));

    // Finally, connect up the network using that user-defined degree distribution
    // Note that poisson, exponential, and scale-free random network generators are built-in
    net.rand_connect_user(degree_dist);

    // Simulation parameters
    int infectious_period = 10; // 10 days, hours,  whatever 
    double T = 0.05; // per timestep, per outbound edge probability of transmission

    for (int i = 0; i < 100; i++){
        // Choose and run simulation
        ChainBinomial_Sim sim(&net, infectious_period, T);
        sim.rand_infect(1);
        sim.run_simulation();
        cout << sim.epidemic_size() << endl;
        sim.reset();
    }
    return 0;
}

