#include "MSMS_Sim.h"
#include <time.h>
#include <stdlib.h>

int j_max; // number of different networks or (repetitions on same network) to iterate through
bool reuse_net; // whether to reuse networks or generate a new one for each j 
int s_max;   // number of times to try each network
int n;     // size of network to use
int patient_zero_ct; // number of infections to introduce (usually 1)
double R_zero; //r0 value for epidemic

enum DistType { POI, EXP, POW, URB, CON}; // specifies which degree distribution to use: poisson, exponential, powerlaw, urban, or constant

DistType dist = POI;

// Distribution parameters.  param2 is a dummy for poisson and exponential.
double param1;
double param2;
int    strains;  // number of strains
double D1;
double D2;
string RunID;

void generate_network(Network* net, MSMS_Sim* sim);
void connect_network (Network* net);

void connect_network (Network* net) {
    if (dist == POI) {
        net->fast_random_graph(param1);
    } else if (dist == EXP) {
        net->rand_connect_exponential(param1);
    } else if (dist == POW) {
        net->rand_connect_powerlaw(param1, param2);
    } else if (dist == URB) {
        vector<double> dist;
        double deg_array[] = {0, 0, 1, 12, 45, 50, 73, 106, 93, 74, 68, 78, 91, 102, 127, 137, 170, 165, 181, 181, 150, 166, 154, 101, 67, 69, 58, 44, 26, 24, 17, 6, 11, 4, 0, 6, 5, 3, 1, 1, 3, 1, 1, 0, 1, 0, 2};
        dist.assign(deg_array,deg_array+47);
        dist = normalize_dist(dist, sum(dist));
        net->rand_connect_user(dist);
    } else if (dist == CON) {
        vector<double> dist(param1+1, 0);
        dist[param1] = 1;
        net->rand_connect_user(dist);
    }
}

void processCmdlineParameter(int argc, char* argv[] ) {
    cerr << "Arguments provided: " << argc - 1 << endl;
    cerr << "Arguments: reps reuse_net seasons net_size r0 dist par1 par2 strain_ct d1 d2 p0_ct run_id\n";
    cerr << "Mixture Model: 0 = none, 1 = half, 2 = full, 3 = natural, 4 = half_shufle, 5 = full_shuffle\n"
         << "Dist: 0 = poisson, 1 = exponential, 2 = powerlaw, 3 = urban, 4 = constant\n";
    if ( argc == 14 ) {  

        j_max     = (int) atoi(argv[1]);
        reuse_net = (bool) atoi(argv[2]);
        s_max     = (int) atoi(argv[3]); 
        n         = (int) atoi(argv[4]);
        R_zero    = (double) atof(argv[5]);
        dist      = (DistType) atoi(argv[6]);
        param1    = (double) atof(argv[7]);
        param2    = (double) atof(argv[8]);
        strains   = (int)    atoi(argv[9]);
        D1        = (double) atof(argv[10]);
        D2        = (double) atof(argv[11]);
        patient_zero_ct =  (int) atoi(argv[12]);
        RunID     = argv[13];
 
        cout << "# "; for(int i=0; i < argc; i++ )  cout << argv[i] <<  " ";  cout << endl;
        //need to check bounds
        if (dist > 4 )    { cerr << "Invalid distribution value\n"; exit(-1); }
        if (D1 < 0)        { cerr << "Expecting positive immune decay parameter\n"; exit(-1); }
        if (D2 < 0)        { cerr << "Expecting positive immune decay parameter\n"; exit(-1); }

    } else {
        cerr << "Expecting 14 parameters.\n";//  Set them in config.pm and use sim.pl to as a wrapper.\n";
        //cerr << "Expecting parameters: <mixture model> <degree dist type> <dist parameter 1> <dist parameter 2 or dummy '-1'> <immune decay parameter>\n";
        exit(-1);
    }
}


int main(int argc, char* argv[]) {
    processCmdlineParameter(argc,argv);

    // Header line
    cout << "# RunID Network Season Epi_size P0_size R0\n";
    if (reuse_net == true) {
        cerr << "Same network topology will be used for all repetitions.\n";
        cerr << "Generating network toplogy . . . ";
    }

    Network* net = new Network("EpiNet", false);
    net->populate(n);
    connect_network(net); // connect network using the parameters above
    MSMS_Sim* sim = new MSMS_Sim(net, strains, D1, D2);
    sim->calc_naive_transmissibility(R_zero); //calculates correct T for this network topology
    vector<Node*> patients_zero;
    double new_R_zero = R_zero;
    for ( int j = 0; j < j_max; j++) {
        cerr << "Repetition: " << j << endl;
        if (reuse_net == false) {
            delete(net);
            delete(sim);
            net = new Network("EpiNet",false);
            net->populate(n);
            connect_network(net); // connect network using the parameters above
            sim = new MSMS_Sim(net, strains, D1, D2);
            sim->calc_naive_transmissibility(R_zero); //calculates correct T for this network topology
        }
        double Tc_actual = sim->calc_critical_transmissibility();

        for ( int season = 0; season < s_max; season++) {
            sim->rand_infect(patient_zero_ct);
            sim->run_simulation();
            cout << RunID << "\t" << j << "\t" << season << "\t";
            for (int strain=0; strain<strains; strain++) {
                cout << sim->epidemic_size(strain) << "\t";
            }
            cout << ((Percolation_Sim*) sim)->epidemic_size() << "\t";
            cout << patients_zero.size() << "\t" << new_R_zero << "\t";
            
            // now calculate what R_zero will be at the start of the next season
            vector<double> average_tk;
            double average_t = 0;
            int strain = 0;
            sim->calculate_average_transmissibility(strain, average_tk, average_t);
            new_R_zero = average_t / Tc_actual;
            cout << endl;
            //sim.summary();
        }
        sim->reset();
        new_R_zero = R_zero;
    }
}
