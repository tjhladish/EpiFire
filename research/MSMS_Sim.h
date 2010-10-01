#ifndef MSMSSIM_H
#define MSMSSIM_H

#include "../src/Percolation_Sim.h"
//#include <assert.h>
//#include <queue>

class MSMS_Sim: public Percolation_Sim
{
    double D1;                 // immune decay parameter for reinfection (T_node = T * exp(-D/G)
    double D2;                 // immune decay parameter for crossinfection (different strain)
    int strains;               // number of strains to use

    inline double immune_decay(double D, int G) {if (G == -1) return 1; return 1 - exp(-D*G);} // Tim's suggestion
    int last_infecting_strain(Node* node) {
        //cerr << "last strain? " << immune_states[node->get_id()][0] << " " << immune_states[node->get_id()][1] << " " << immune_states[node->get_id()][2] << "\t"; 
        int min_idx = -1;
        int min = INT_MAX;
        for (int j = 0; j<strains; j++) {
            int val = immune_states[node->get_id()][j];
            if ( val > -1 and val < min) {
                min = val;
                min_idx = j;
            }
        }
        //min = min == INT_MAX ? -1 : min; // -1 means never infected
        //cerr << min << endl;
        return min_idx;
    }
    //inline int last_infecting_strain(Node* node) {return node->get_state() - 1;} // -1 means never infected

    vector< vector<int> > immune_states; // for each node, time since last infection for each strain
    // e.g., immune_states[300][1] == time for node 300 with respect to strain 1

    public:
        MSMS_Sim():Percolation_Sim() {}
        MSMS_Sim(Network* net, int strains, double D1, double D2):Percolation_Sim(net) { this->D1 = D1; this->D2 = D2; this->strains = strains; build_immune_state_var(); }

        void set_immune_decay_par( double D1, double D2 ) { this->D1 = D1; this->D2 = D2; }
        double calc_naive_transmissibility( double R_zero ) { this->T = R_zero * calc_critical_transmissibility(); return T; }
        double get_naive_transmissibility() { return T; }

        void build_immune_state_var() {
            for (int i=0; i<net->size(); i++) {
                vector<int> times(strains, -1);
                immune_states.push_back(times);
            }
        }

        double get_transmissibility(int source_strain, Node* dest) { // uses the exponential decay model of immunity loss
            int id = dest->get_id();
            int last_strain = last_infecting_strain(dest);           // what strain does this node have most immunity to?
            last_strain = last_strain == -1 ? source_strain : last_strain; // if -1, node has never been infected, so we can use any legit value
            int G = immune_states[ id ][ last_strain ];  // when was that?
            //cerr << "id source dest time susc " << id << " " << source_strain << " " << last_strain << " " << G << " ";

            if ( last_strain > -1 and immune_states[ id ][ last_strain ] == 0 ) {/* cerr << "0\n";*/ return 0.0;} // already infected, and it was this season; not S
            
            if (source_strain == last_strain) { // reinfection by same strain
//                cerr << immune_decay(D1, G) << endl;
                return (double) (T * immune_decay(D1, G)); // D is the immune decay parameter, G is seasons since last infection 
            } else {                            // crossinfection by different strain
//                cerr << immune_decay(D2, G) << endl;
                return (double) (T * immune_decay(D2, G));
            }
        }


        void calculate_average_transmissibility(int strain, vector<double> &average_tk, double &average_t) {
            average_t = 0;
            vector<int> deg_dist = net->get_deg_dist();
            vector<double> tk( deg_dist.size() );    // total transmissibility by degree -- used to calc mean
            vector<Node*> nodes = net->get_nodes();

            for (int i = 0; i < (signed) nodes.size(); i++) {
                int deg = nodes[i]->deg();
                tk[deg] += get_transmissibility(strain, nodes[i]);
            }

            average_tk.resize( tk.size() ); // average transmissibility of degree k nodes
            int deg_sum = sum(net->get_deg_series()); // sum of all degrees in the network, i.e. the total number of stubs
            for (int deg = 0; deg < (signed) tk.size(); deg++) {
                average_tk[deg] = tk[deg]/ ((double) deg_dist[deg]);
                average_t += tk[deg] * deg / ((double) deg_sum);
            }
        }

        void run_simulation() {
            recovered.clear();
            vector<Node*> nodes = net->get_nodes();
            while (infected.size() > 0) { //let the epidemic spread until it runs out
                step_simulation();
            }

            for ( int i = 0; i < (signed)  nodes.size(); i++ ) {
//if (i<5) cerr << "[] " << i << " [" << immune_states[i][0]<< " " << immune_states[i][1]  << " " << immune_states[i][2] << "]\n";
                for ( int j = 0; j<strains; j++ ) {
                    if ( immune_states[ i ][ j ] >= 0 ) { // if this node has been infected
                        immune_states[ i ][ j ]++;        // (if it's susceptible, it stays at -1)
                    }
                }
            }
            infected.clear();
        }


        // For each degree, count the number of nodes that have each state
        vector< vector<int> > tabulate_states() {
            vector< vector<int> > states_by_degree = net->get_states_by_degree();
            vector< vector<int> > tabulated(states_by_degree.size());
            for (int d = 0; d<(signed) states_by_degree.size(); d++) tabulated[d] = tabulate_vector(states_by_degree[d]);
            return tabulated;
        }
        
        int epidemic_size( int strain) {
            int epi_size = 0;
            for (unsigned int i = 0; i<immune_states.size(); i++) {
                if (immune_states[i][strain] == 1) epi_size++;
            }
            return epi_size;
        }


        void rand_infect (int n) { // not quite perfect; this currently allows coinfections
                                   // at least in terms of immunity.  Should rarely be an issue
                                   // as long as p0 class is small relative to net size
            assert(n > 0);
            for (int j=0; j<strains; j++) {
                vector<Node*> patients_zero = rand_set_nodes_to_state(n, j);
                for (int i = 0; i < n; i++) {
                    Node* node = patients_zero[i];

                    infected.push_back(node);
            //        cerr << "id strain " << node->get_id() << " " << j << endl;
                    immune_states[ node->get_id() ][ j ] = 0;
                }
            }
        }

        void reset() {
            Percolation_Sim::reset();
            build_immune_state_var();
        }

    private:
        void step_simulation () { // private, because it does not increment states
            time++;
            assert(infected.size() > 0);
            vector<Node*> new_infected;
            for (int i = 0; i < (signed) infected.size(); i++) {
                Node* inode = infected[i];
                vector<Node*> neighbors = inode->get_neighbors();
                for (int j = 0; j < (signed) neighbors.size(); j++) {
                    Node* test = neighbors[j];
                    int strain = last_infecting_strain(inode);
                    int s = inode->get_id(); int d = test->get_id();
        //            cerr << "source node: [" << immune_states[s][0]<< " " << immune_states[s][1]  << " " << immune_states[s][2] << "]\n";
          //          cerr << "dest   node: [" << immune_states[d][0]<< " " << immune_states[d][1]  << " " << immune_states[d][2] << "]\n";
            //        cerr << "T, infecting_strain: " << get_transmissibility(strain, test) << " " << strain << endl;
                    if ( mtrand->rand() < get_transmissibility(strain, test) ) {
                        immune_states[ test->get_id() ][ strain ] = 0;
                        new_infected.push_back( test );
                    }
              //      cerr << "result:      [" << immune_states[d][0]<< " " << immune_states[d][1]  << " " << immune_states[d][2] << "]\n\n";
                }

                recovered.push_back(inode);
            }
            infected = new_infected;
        }
};

#endif
