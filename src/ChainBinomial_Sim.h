#ifndef  CHAIN_SIMULATOR_H
#define  CHAIN_SIMULATOR_H

#include "Simulator.h"

class ChainBinomial_Sim: public Simulator
{
    protected:
        list<Node*> infected;
        vector<Node*> new_infected;
        vector<Node*> recovered;

    public:
        double T;                // transmissibiltiy per time step
        int infectious_period;

        ChainBinomial_Sim() { this->time = 0; };
        ChainBinomial_Sim(Network* net, int infectious_period, double T):Simulator(net) { this->infectious_period=infectious_period; this->T=T; };

        void set_infectious_period(int d) { this->infectious_period = d; }
        void set_transmissibility(double t) { this->T = t; }

        int    get_infectious_period() { return this->infectious_period; }
        double get_transmissibility()  { return this->T; }

        vector<Node*> rand_infect (int n) {
            assert(n > 0);
            vector<Node*> patients_zero = rand_set_nodes_to_state(n, 1);
            for (int i = 0; i < n; i++) {
                infected.push_back(patients_zero[i]);
            };
        }

        void step_simulation () {
            // States: 0 (default) is susceptible
            //         1 is infectious day 1
            //         2 is infectious day 2
            //         ... up to the infectious period
            //         -1 is recovered
            assert(infected.size() > 0);
            time++;
            new_infected.clear();
            
            list<Node*>::iterator inode;
            for (inode=infected.begin(); inode!=infected.end(); inode++) {
                
                vector<Node*> neighbors = (*inode)->get_neighbors();
                for (unsigned int j = 0; j < neighbors.size(); j++) {
                    Node* test = neighbors[j];
                    // Is this neighbor susceptible and 
                    // has it been exposed?
                    if ( test->get_state() == 0 && mtrand->rand() < T ) {
                        set_node_state( test, 1 );
                        new_infected.push_back( test );
                    }
                }
                // Increment node states through the infectious period
                (*inode)->set_state( (*inode)->get_state()+1 );
            }

            // Some nodes may be reaching the end of their infectious period ...
            while (infected.size() > 0 && infected.front()->get_state() > infectious_period) {
                Node* first = infected.front();
                set_node_state(first, -1); // -> recovered
                recovered.push_back( first );
                infected.pop_front();
            }
           
            // Append the newly infected to the queue
            for (unsigned int i = 0; i<new_infected.size(); i++) {
                infected.push_back( new_infected[i] );
            }

        }

        void run_simulation() {
            assert(infectious_period > 0 && T >= 0 && T <= 1);
            vector<Node*> nodes = net->get_nodes();
            
            // As long as someone's still infected, step simulation
            while (infected.size() > 0)  step_simulation();

        }

        int count_infected() {
            return infected.size();
        }

        int epidemic_size() {
            return recovered.size();
        }

        void reset() {
            reset_time();

            while (! infected.empty()) {
                set_node_state(infected.front(), 0);
                infected.pop_front();
            }

            set_these_nodes_to_state(new_infected, 0);
            new_infected.clear();

            set_these_nodes_to_state(recovered, 0);
            recovered.clear();
        }

        void summary() {
            cerr << "Network size: " << net->size();
            cerr << "\tTransmissibility: " << T;
            cerr << "\tEpidemic size: " << recovered.size() << "\n\n";

        }
};
#endif
