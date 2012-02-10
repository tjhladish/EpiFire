#ifndef PERCOL_SIMULATOR_H
#define PERCOL_SIMULATOR_H

#include "Simulator.h"

class Percolation_Sim: public Simulator
{
    protected:
        vector<Node*> infected;
        vector<Node*> recovered;

    public:
        typedef enum {           //Whatever is equal to zero is the default state
            S=0, I=1, R=-1
        } stateType;
        float T;                 // transmissibiltiy, e.g. P{infection spreading along a given edge that connects an infected and a susceptible}

        Percolation_Sim():Simulator() {};
        Percolation_Sim(Network* net):Simulator(net) {};
        ~Percolation_Sim() { };

        void set_transmissibility(double t) { this->T = t; }

        double expected_R0 () {
            //assert(T != NULL);
            return T / calc_critical_transmissibility();
        }

        vector<Node*> rand_infect (int n) {
            assert(n > 0);
            vector<Node*> patients_zero = rand_set_nodes_to_state(n, I);
            for (int i = 0; i < n; i++) {
                infected.push_back(patients_zero[i]);
            };
            return patients_zero;
        }

        void step_simulation () {
            assert(infected.size() > 0);
            time++;
            //cerr << "\t" << infected.size() << endl;
            vector<Node*> new_infected;
            for (unsigned int i = 0; i < infected.size(); i++) {
                Node* inode = infected[i];
                vector<Node*> neighbors = inode->get_neighbors();
                for (unsigned int j = 0; j < neighbors.size(); j++) {
                    Node* test = neighbors[j];
                    if ( test->get_state() == S && mtrand->rand() < T ) {
                        test->set_state( I );
                        new_infected.push_back( test );
                    }
                }
                inode->set_state( R );
                recovered.push_back( inode );
            }
            infected = new_infected;
        }

        void run_simulation() {
            while (infected.size() > 0) {
                step_simulation();
            }
        }

        int count_infected() {
            return infected.size();
        }

        int epidemic_size() {
            return recovered.size();
        }

        void reset() {
            reset_time();

            set_these_nodes_to_state(infected, S);
            infected.clear();

            set_these_nodes_to_state(recovered, S);
            recovered.clear();
        }

        void summary() {
            cerr << "Network size: " << net->size();
            cerr << "\tTransmissibility: " << T;
            cerr << "\tEpidemic size: " << recovered.size() << "\n\n";

        }
};
#endif
