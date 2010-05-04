#ifndef GUI_SIM_H
#define GUI_SIM_H

#include "../src/Percolation_Sim.h"
#include <time.h>
#include <stdlib.h>
#include <QApplication>


enum DistType  { POI, EXP, POW, URB, CON};
vector< vector<int> > simulate_main(int j_max, bool reuse_net, int n, double R_zero, DistType dist, double param1, double param2, int patient_zero_ct, string RunID, int* dist_size_loc);

#endif
