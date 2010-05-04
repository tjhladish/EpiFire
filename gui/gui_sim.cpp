#include <gui_sim.h>

/*
int j_max; // number of different networks or (repetitions on same network) to iterate through
bool reuse_net; // whether to reuse networks or generate a new one for each j 
int n;     // size of network to use
int patient_zero_ct; // number of infections to introduce (usually 1)
double R_zero; //r0 value for epidemic

// Distribution parameters.  param2 is a dummy for poisson and exponential.
DistType dist;
double param1;
double param2;
string RunID;
*/
 // specifies which degree distribution to use: poisson, exponential, powerlaw, urban, or constant
//void connect_network (Network* net, DistType dist, double param1, double param2);

void connect_network (Network* net, DistType dist, double param1, double param2) {
    if (dist == POI) {
        net->rand_connect_poisson(param1);
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

void generate_network(Network* net, int n, DistType dist, double param1, double param2, double R_zero, Percolation_Sim* sim) {
    net->populate(n);
    connect_network(net, dist, param1, param2); // connect network using the parameters above
    double T = R_zero * sim->calc_critical_transmissibility();
    sim->set_transmissibility(T);
}

vector< vector<int> > simulate_main(int j_max, bool reuse_net, int n, double R_zero, DistType dist, double param1, double param2, int patient_zero_ct, string RunID, int* dist_size_loc) {
    // Header line
    //cout << "# RunID Network Season Epi_size P0_size R0\n";
    if (reuse_net == true) {
        cerr << "Same network topology will be used for all repetitions.\n";
        cerr << "Generating network toplogy . . . \n";
    }

    Network* net = new Network("EpiNet", false);
    Percolation_Sim* sim = new Percolation_Sim(net);

    generate_network(net, n, dist, param1, param2, R_zero, sim);
    vector<Node*> patients_zero;
    vector< vector<int> > epi_curves (j_max);
    
    for ( int j = 0; j < j_max; j++) {
        if (reuse_net == false) {
            delete(net);
            delete(sim);
            net = new Network("EpiNet",false);
            sim = new Percolation_Sim(net);
            generate_network(net, n, dist, param1, param2, R_zero, sim);
        }

        //the following line isn't compiling in OSX for some reason
        //patients_zero = sim->rand_infect(patient_zero_ct);
        sim->rand_infect(patient_zero_ct);
        
        vector<int> epi_curve; 
        epi_curve.push_back(sim->infected.size());
        //sim->run_simulation();
        while (sim->infected.size() > 0) {
            sim->step_simulation();
            epi_curve.push_back(sim->infected.size());
        }
        
        for(unsigned int k=0; k < epi_curve.size(); k++ ) cerr << epi_curve[k]<<endl;
        
        //cout << RunID << " " << j << " " << sim->epidemic_size() << " " << patients_zero.size() << " ";
        cout << "Rep: " << j << "    Total: " << sim->epidemic_size() << "\n\n";
        epi_curves[j] = epi_curve; 
        
        //plotArea->addData(epi_curve);
	//QString simOutput;

	//Use pointer to report epidemic size back to Dialog class
	*dist_size_loc=sim->epidemic_size();
	dist_size_loc++;

        //cout << RunID << " " << j << " " << sim->epidemic_size() << " " << patients_zero.size() << " ";
        cout << RunID << " " << j << " " << sim->epidemic_size() << " ";

        //sim.summary();
        sim->reset();
    }
    delete(net);
    delete(sim);
    return epi_curves;
}

