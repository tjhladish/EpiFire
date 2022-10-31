#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Network.h"
#include <assert.h>
#include <queue>

class Simulator
{

    public:

        int time;
        Network* net;
        std::mt19937* rng;

        Simulator() { time = 0; net=NULL; rng=NULL; };
        Simulator(Network* net) { this->net = net; this->time = 0; this->rng = net->get_rng(); };

        void set_network( Network* net ) { this->net = net; this->rng = net->get_rng(); };
        Network* network() { return(net); };

        int get_time() { return(time); };

        void reset_time() { time = 0; };

        void set_all_nodes_to_state ( stateType s ) {
            vector<Node*> nodes = net->get_nodes();
            set_these_nodes_to_state(nodes, s);
        };

        void set_these_nodes_to_state (vector<Node*> nodes, stateType s) {
            for (unsigned int i = 0; i < nodes.size(); i++) nodes[i]->set_state(s);
        }

        // choose n nodes without replacement
        vector<Node*> rand_choose_nodes (int n) {
            assert(n > -1 and n <= net->size());
            vector<Node*> nodes = net->get_nodes();
            vector<Node*> sample(n);
            vector<int> sample_ids(n);
            rand_nchoosek(net->size(), sample_ids, rng);
            Node* node;
            for (unsigned int i = 0; i < sample_ids.size(); i++) {
                node = nodes[ sample_ids[i] ];
                sample[i] = node;
            };
            return sample;
        }

        // change n random nodes to state s (e.g. vaccinate them or infect them randomly)
        vector<Node*> rand_set_nodes_to_state (int n, stateType state) {
            vector<Node*> sample = rand_choose_nodes(n);
            for (unsigned int i = 0; i < sample.size(); i++) {
                sample[i]->set_state(state);
            };
            return sample;
        }

        double calc_critical_transmissibility() {
            vector<double> dist = net->get_gen_deg_dist();
            double numerator = 0;// mean degree, (= <k>)
                                 // mean sq(deg) - mean deg (= <k^2> - <k>)
            double denominator = 0;
            for (unsigned int k=1; k < dist.size(); k++) {
                numerator += k * dist[k];
                denominator += k * (k-1) * dist[k];
            }
            return numerator/denominator;
        }


        //these functions must be derived in child class
        virtual ~Simulator() {};
        virtual void step_simulation() {};
        virtual void run_simulation() {};
        virtual vector<Node*> rand_infect(int) {vector<Node*>x; return x;};
        virtual int epidemic_size() = 0;  //pure virtual - must be derived in a child
        virtual int count_infected() {
            return 0;
        };
        virtual void reset() {};

};

/*
class Derived_Example_Simulator: public Simulator {

    public:
        Derived_Example_Simulator();
        Derived_Example_Simulator(Network* net);
        ~Derived_Example_Simulator();

        void step_simulation() { time++; do_stuff_to_network;  };
        void run_simulation() { while(conditional) { step_simulation() } };

};
*/
#endif
