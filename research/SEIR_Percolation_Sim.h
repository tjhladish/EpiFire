#ifndef SEIR_PERCOL_SIMULATOR_H
#define SEIR_PERCOL_SIMULATOR_H

#include "Percolation_Sim.h"

class SEIR_Percolation_Sim: public Percolation_Sim
{

    public:
        typedef enum {           //Whatever is equal to zero is the default state
            S=0, E=1, I=2, R=3
        } stateType;
        
       // vector<Node*> infected;
       // vector<Node*> recovered;
        vector<Node*> exposed;

        SEIR_Percolation_Sim():Percolation_Sim() {};
        SEIR_Percolation_Sim(Network* net):Percolation_Sim(net) {};

        void rand_expose (int n) {
            assert(n > 0);
            vector<Node*> patients_zero = rand_set_nodes_to_state(n, E);
            for (int i = 0; i < n; i++) {
                exposed.push_back(patients_zero[i]);
            };
        }
        
        void step_simulation () {
            time++;
            
            // print which iteration we're on
            string s = to_string(time);
            s.resize(5, ' ');
            cout << s;

            print_states();
            vector<Node*> new_exposed;
            for (int i = 0; i < infected.size(); i++) {
                Node* inode = infected[i];
                vector<Node*> neighbors = inode->get_neighbors();
                for (int j = 0; j < neighbors.size(); j++) {
                    Node* test = neighbors[j];
                    if ( test->get_state() == S && mtrand->rand() < T ) {
                        test->set_state( E );
                        new_exposed.push_back( test );
                    }
                }
                inode->set_state( R );
                recovered.push_back( inode );
            }
            
            // Move the old exposed class to the infected class 
            for (int i = 0; i < exposed.size(); i++) {
                exposed[i]->set_state( I );
            }
            infected = exposed;
            exposed = new_exposed;
        }

        void run_simulation() {
            //as long as there are exposed or infected continue with simulation
            while (infected.size() > 0 || exposed.size() > 0) {
                step_simulation();
            }
        }
        
        void reset() {
            reset_time(); // time back to zero

            // make everyone susceptible again
            set_these_nodes_to_state(exposed, S);
            exposed.clear();

            set_these_nodes_to_state(infected, S);
            infected.clear();

            set_these_nodes_to_state(recovered, S);
            recovered.clear();
        }

        void print_states() { 
            char printChars[4] = { ' ', '.' , '*' , 'O' };
            for(int i=0; i < net->size(); i++ ) {
                Node* node = net->get_node(i);
                cout << printChars[ node->get_state() ];
            }
            cout << endl;
        }


};

#endif
