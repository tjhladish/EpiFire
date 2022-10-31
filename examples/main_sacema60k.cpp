#include "Gillespie_MassAction_Sim.h"
#include "AbcSmc.h"

#include <gsl/gsl_rng.h>
#include <math.h>
#include <unistd.h>

using namespace std;
const gsl_rng* RNG = gsl_rng_alloc(gsl_rng_taus2);

vector<double> simulator(vector<double> args, const unsigned long int rng_seed, const unsigned long int serial, const ABC::MPI_par* mp) {
    int N     = 1000;
    double BETA  = args[0];
    double GAMMA = args[1];

    double peak_height = -1.0;
    const size_t max_time = 75;
    Gillespie_MassAction_Sim sim(N,GAMMA,BETA);
    sim.rng.seed(rng_seed); //seed RNG for simulator
    sim.rand_infect(10);
    double prev_recent = (double) sim.Compartments[1] / N;
    vector<double> prev_curve = {prev_recent}; // prevalence at t=0

    while (sim.next_event()) {
        const double prev_now = (double) sim.Compartments[1] / N;
        const size_t time_step = (size_t) sim.Now;
        if ( time_step >= prev_curve.size() ) prev_curve.resize(time_step+1, prev_recent);
        prev_recent = prev_now;
        
        if (sim.Now > max_time) break;
    }
    prev_curve.resize(max_time+1, 0.0);

    //cout << "Prevalence at T=50, T=75, peak: " << prev_at_50 << "\t" << prev_at_75 << "\t" << peak_height << endl;
    //vector<double> metrics = {prev_at_50, prev_at_75, peak_height};    
//    for (auto v: prev_curve) cout << v << endl;
//    cout << "prev curve size: " << prev_curve.size() << endl;

//    75k particle scenario:
//    vector<double> metrics = {prev_curve[50], prev_curve[75], *max_element(prev_curve.begin(), prev_curve.end())};

//    60k particle scenario
    vector<double> metrics = {prev_curve[50], prev_curve[75]};
//    for (auto v: metrics) cout << v << " " ; cout << endl;

    return metrics;
}


void usage() {
    cerr << "\n\tUsage: ./abc_sql abc_config_sql.json --process\n\n";
    cerr << "\t       ./abc_sql abc_config_sql.json --simulate\n\n";
    cerr << "\t       ./abc_sql abc_config_sql.json --simulate -n <number of simulations per database write>\n\n";
    cerr << "\t       ./abc_sql abc_config_sql.json --process --simulate -n <number of simulations per database write>\n\n";
    cerr << "\t       ./abc_sql abc_config_sql.json --all\n\n";

}


int main(int argc, char* argv[]) {

    if (not (argc == 3 or argc == 5 or argc == 6) ) {
        usage();
        exit(100);
    }

    bool process_db = false;
    bool simulate_db = false;
    bool do_all      = false;
    int buffer_size = -1;

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

    AbcSmc* abc = new AbcSmc();
    abc->parse_config(string(argv[1]));
    size_t set_count = do_all ? abc->get_smc_iterations() : 1;

    for (size_t i = 0; i < set_count; ++i) {
        if (do_all) {
            buffer_size = (int) abc->get_num_particles(i, QUIET);
        }
        if (process_db) {
            gsl_rng_set(RNG, time(NULL) * getpid()); // seed the rng using sys time and the process id
            abc->process_database(RNG);
        }

        if (simulate_db) {
            abc->set_simulator(simulator);
            abc->simulate_next_particles(buffer_size);
        }
    }

    if (do_all) {
        abc->process_database(RNG); // one last time, to get the posterior
    }

    return 0;
}
