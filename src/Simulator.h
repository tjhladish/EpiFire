#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Network.h"
#include <assert.h>
#include <queue>
#include <deque>

class Event {
    public:
        Event(int t, Node* n, stateType s) { time=t; node=n; state=s; }     //1
        Event(const Event& b) { time=b.time; node=b.node; state=b.state; }   //2
        Event& operator= (const Event& b) { time=b.time; node=b.node; state=b.state; } //3

    int time;
    Node* node;
    stateType state;
};

class Simulator
{

    public:

        int time;
        Network* net;
        MTRand* mtrand;
        deque<Event>events;

        Simulator() { time = 0; net=NULL; mtrand=NULL; };
        Simulator(Network* net) { this->net = net; this->time = 0; this->mtrand = net->get_rng(); };

        void set_network( Network* net ) { this->net = net; this->mtrand = net->get_rng(); };
        Network* network() { return(net); };

        int get_time() { return(time); };

        void reset_time() { time = 0; };

        void set_all_nodes_to_state ( stateType s ) {
            vector<Node*> nodes = net->get_nodes();
            set_these_nodes_to_state(nodes, s);
        };

        void set_these_nodes_to_state (vector<Node*> nodes, stateType s) {
            for (unsigned int i = 0; i < nodes.size(); i++) set_node_state(nodes[i], s);
        }

        void set_node_state(Node* node, stateType state) { 
            node->set_state(state); 
            add_event(get_time(),node,state); 
        }

        // change n random nodes to state s (e.g. vaccinate them or infect them randomly)
        vector<Node*> rand_set_nodes_to_state (int n, stateType state) {
            assert(n > -1);
            vector<Node*> nodes = net->get_nodes();
            vector<Node*> sample(n);
            vector<int> sample_ids(n);
            rand_nchoosek(net->size(), sample_ids, mtrand);
            Node* node;
            for (unsigned int i = 0; i < sample_ids.size(); i++) {
                node = nodes[ sample_ids[i] ];
                set_node_state(node, state);
                sample[i] = node;
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
        virtual void step_simulation() {};
        virtual void run_simulation() {};
        virtual vector<Node*> rand_infect(int) {vector<Node*>x; return x;};
                                 // cumulative infected
        virtual int epidemic_size() = 0;
                                 // current infected
        virtual int count_infected() {
            return 0;
        };
        virtual void reset() {};

        void add_event(int t, Node* n, stateType s)  { events.push_back(Event(t,n,s)); }

        virtual vector<int> epidemic_curve() {vector<int>x; return x;}

        vector< vector<Event*> > get_history_matrix() { 
             vector< vector<Event*> > history( get_time()+1 );
             for(int i=0; i < events.size(); i++ ) {
                if ( events[i].time > get_time() + 1 ) {
                    cerr << "WOOOPs " << events[i].time << endl;
                }


                history[ events[i].time ].push_back( &events[i] );
             }
             return history;
        }
        

        void write_event_history(string filename) {
            ofstream fout(filename.c_str(), ios::out);
            for(unsigned int i=0; i< events.size(); i++ ) {
                  fout << events[i].time << "\t" 
                       << events[i].node << "\t "
                       << events[i].state << endl;
            }
            fout.close();
        }

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
