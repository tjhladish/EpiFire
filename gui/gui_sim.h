#include "../src/Percolation_Sim.h"
#include <time.h>
#include <stdlib.h>
#include <QApplication>


enum DistType  { POI, EXP, POW, URB, CON};
void connect_network (Network* net, DistType dist, double param1, double param2);
void generate_network(Network* net, int n, DistType dist, double param1, double param2, double R_zero, Percolation_Sim* sim);
vector< vector<int> > simulate_main(int j_max, bool reuse_net, int n, double R_zero, DistType dist, double param1, double param2, int patient_zero_ct, string RunID);

