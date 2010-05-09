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

        void rand_infect (int n) {
            assert(n > 0);
            vector<Node*> patients_zero = rand_set_nodes_to_state(n, 1);
            for (int i = 0; i < n; i++) {
                infected.push_back(patients_zero[i]);
            };
        }

        void step_simulation () {
            assert(infected.size() > 0);
            time++;
            //cerr << "\t" << infected.size() << endl;
            new_infected.clear();
            list<Node*>::iterator it;
            for (it=infected.begin(); it!=infected.end(); it++) {
                Node *inode = *it;
                vector<Node*> neighbors = inode->get_neighbors();
                for (int j = 0; j < neighbors.size(); j++) {
                    Node* test = neighbors[j];
                    if ( test->get_state() == 0 && mtrand->rand() < T ) {
                        test->set_state( 1 );
                        new_infected.push_back( test );
                    }
                }
                inode->set_state( inode->get_state()+1 );
            }
            while (infected.size() > 0 && infected.front()->get_state() > infectious_period) {
                Node* first = infected.front();
                first->set_state(-1);
                recovered.push_back( first );
                infected.pop_front();
            }
            for (int i = 0; i<new_infected.size(); i++) {
                infected.push_back( new_infected[i] );
            }
        }

        void run_simulation() {
            assert(infectious_period > 0 && T >= 0 && T <= 1);
            vector<Node*> nodes = net->get_nodes();
                                 // verify that this is right
            for (int i = 0; i<nodes.size(); i++) {
                                 //
                nodes[i]->set_state(0);
            }                    //
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

            //set_these_nodes_to_state(infected, 0);
            //infected.clear();

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
