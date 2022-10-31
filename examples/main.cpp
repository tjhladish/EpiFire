#include "Gillespie_MassAction_Sim.h"
#include "AbcSmc.h"

#include <gsl/gsl_rng.h>
//#include <gsl/gsl_statistics_double.h>
#include <math.h>
#include <unistd.h>
#include <algorithm>

using namespace std;
const gsl_rng* RNG = gsl_rng_alloc(gsl_rng_taus2);

vector<double> simulator(vector<double> args, const unsigned long int rng_seed) {
    int N     = 1000;
    double BETA  = args[0];
    double GAMMA = args[1];

    double peak_height = -1.0;
    const size_t max_time = 75;
//    double prev_at_50  = -1.0;
//    double prev_at_75  = -1.0;
    Gillespie_MassAction_Sim sim(N,GAMMA,BETA);
    sim.rng.seed(rng_seed); //seed RNG for simulator
    sim.rand_infect(10);
    double prev_recent = (double) sim.Compartments[1] / N;
    vector<double> prev_curve = {prev_recent}; // prevalence at t=0

    while (sim.next_event()) {
        const double prev_now = (double) sim.Compartments[1] / N;
        const size_t time_step = (size_t) sim.Now;
        if ( time_step >= prev_curve.size() ) prev_curve.resize(prev_recent, time_step+1);
        prev_recent = prev_now;

        //if (prev_now > peak_height) peak_height = prev_now;
        
        /*if ((int) sim.Now > 50 and prev_at_50 == -1)  { // TODO -- report values just before 50 and 75, not just after
            prev_at_50 = prev_now;
        } else if ((int) sim.Now > 75 and prev_at_75 == -1)  {
            prev_at_75 = prev_now;
            break;
        }*/
        if (sim.Now > max_time) break;
    }
    //cout << "Prevalence at T=50, T=75, peak: " << prev_at_50 << "\t" << prev_at_75 << "\t" << peak_height << endl;
    //vector<double> metrics = {prev_at_50, prev_at_75, peak_height};    
    for (auto v: prev_curve) cout << v << endl;
    vector<double> metrics = {prev_curve[50], prev_curve[75], *max_element(prev_curve.begin(), prev_curve.end())};    

    return metrics;
}


int main(int argc, char* argv[]) {
    for (int i=2; i < argc;  i++ ) {
        if ( strcmp(argv[i], "--process") == 0  ) {
            process_db = true;
        } else if ( strcmp(argv[i], "--simulate") == 0  ) {
            simulate_db = true;
            buffer_size = buffer_size == -1 ? 1 : buffer_size;
        } else if ( strcmp(argv[i], "--all") == 0  ) {
            do_all = true;
            process_db = true;
            simulate_db = true;
        } else if ( strcmp(argv[i], "-n" ) == 0 ) {
            buffer_size = atoi(argv[++i]);
        } else {
            usage();
            exit(101);
        }
    }
    return 0;
}
