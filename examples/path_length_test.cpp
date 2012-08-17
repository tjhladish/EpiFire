#include <Network.h>
#include <iostream>
#include <cstdio>
#include <time.h>

using namespace std;

int main() {
    vector<int> sizes;
    sizes.push_back(50);
    sizes.push_back(100);
    sizes.push_back(200);
    sizes.push_back(1000);

    for ( int i = 0; i<sizes.size(); i++) {
        Network net("poisson", Network::Undirected); 
        net.populate(sizes[i]);
        net.fast_random_graph(10);

        vector<Node*> dummy_vector;

        int start = clock();
        cerr << net.mean_dist(dummy_vector);
        int stop = clock();

        cerr << "\tTime = " << ((float)stop - start)/CLOCKS_PER_SEC << endl;
    }
    return 0;
}
