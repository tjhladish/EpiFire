#include <ChainBinomial_Sim.h>

int main() {
    // Construct Network
    Network net("name", Network::Undirected);
    Network::seed(); //seed RNG (can pass in a custom seed)

    net.populate(4000);
    net.small_world(4000, 12, 0.2675);
    ChainBinomial_Sim cs(&net,16,0.007);
    cout << "R0: " << cs.expected_R0() << endl;

    /*// Now make those probabilities that sum to 1
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
    }*/
    return 0;
}

