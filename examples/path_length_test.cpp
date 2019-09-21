#include <Network.h>
#include <iostream>
#include <cstdio>
#include <time.h>

using namespace std;

int main() {
    Network net(Network::Undirected);
    net.populate(7);
    vector<Node*> nodes = net.get_nodes();
    nodes[0]->connect_to(nodes[1]);
    nodes[0]->connect_to(nodes[2]);
    nodes[0]->connect_to(nodes[3]);
    nodes[1]->connect_to(nodes[2]);
    nodes[3]->connect_to(nodes[4]);
    nodes[3]->connect_to(nodes[5]);
    nodes[5]->connect_to(nodes[6]);
    //net.fast_random_graph(2);

    PairwiseDistanceMatrix dist_map = net.calculate_distances_map();

    /*
    0 1 1 1 2 2 3
    1 0 1 2 3 3 4
    1 1 0 2 3 3 4
    1 2 2 0 1 1 2
    2 3 3 1 0 2 3
    2 3 3 1 2 0 1
    3 4 4 2 3 1 0
    */

    for (pair<const Node*, DistanceMatrix> dm: dist_map) {
        for (pair<const Node*, double> nd: dm.second) cerr << nd.second << " ";
        cerr << endl;
    }
    vector<int> sizes = {50, 100, 200, 1000};

    for (int size: sizes) {
        Network net(Network::Undirected);
        net.populate(size);
        net.fast_random_graph(10);

        int start = clock();
        cerr << net.mean_dist();
        int stop = clock();
        cerr << "\tFor Poisson random graph of size " << size << ",\tTime = " << ((float)stop - start)/CLOCKS_PER_SEC << endl;
    }

    return 0;
}
