#ifndef  CHAIN_SIMULATOR_H
#define  CHAIN_SIMULATOR_H

#include "Simulator.h"
#include <queue>

class Event {

    public:
        Node* sink_node;
        int time;
        Node* source_node;
        Event(const Event& o) {  sink_node=o.sink_node; time = o.time; source_node = o.source_node; }
        Event(Node* sink, int t, Node* source) {  sink_node=sink; time=t; source_node=source; }
        Event& operator=(const Event& o) {
            if (this != &o) {
                sink_node=o.sink_node; time = o.time; source_node = o.source_node;
            }
            return *this;
        }

};

class compTime {
    public:
        bool operator() (const Event* lhs, const Event* rhs) const {
            return (lhs->time>rhs->time);
        }

        bool operator() (const Event& lhs, const Event& rhs) const {
            return (lhs.time>rhs.time);
        }
};


class ChainBinomial_Sim: public Simulator
{
    protected:
        list<Node*> infected;
        vector<Node*> recovered;
        vector<double> time_dist; // Probability mass function for day of transmission
        bool update_time_dist;
        priority_queue<Event, vector<Event>, compTime > transmissionQ;
        
        vector< int > epi_curve;  // new infections at each time step (incidence)
        vector< pair<int, Node*> > detailed_epi_curve; // track node->infected node state changes as (time, node_id) pairs

    public:
        double T;                // transmissibiltiy per time step
        int infectious_period;

        ChainBinomial_Sim():Simulator() { this->time = 0; this->update_time_dist=true;};
        ChainBinomial_Sim(Network* net, int infectious_period, double T):Simulator(net) { this->infectious_period=infectious_period; this->T=T; define_time_dist();};

        void set_network(Network* net) { this->net=net; }
        void set_infectious_period(int d) { this->infectious_period = d; this->update_time_dist=true;}
        void set_transmissibility(double t) { this->T = t; this->update_time_dist=true;}

        vector<double> define_time_dist() {
            time_dist.clear();
            for (int i = 0; i < infectious_period; i++) {
                time_dist.push_back( pow(1-T, i) * T );
            }
            time_dist.push_back( pow(1-T, infectious_period) );
            this->update_time_dist = false;
            return time_dist;
        }

        int    get_infectious_period() { return this->infectious_period; }
        double get_transmissibility()  { return this->T; }

        vector<Node*> rand_infect (int n) { // randomly infect n nodes
            assert(n > 0);
            vector<Node*> patients_zero = rand_choose_nodes(n);//set_nodes_to_state(n, 1);
            for (int i = 0; i < n; i++) infect_node(patients_zero[i]);
            return patients_zero;
        }

        void infect_node(Node* node) {
            if (node->get_state() != 0) return; //already infected or recovered
            node->set_state(1);
            infected.push_back(node);
            epi_curve.resize(time+1, 0);
            epi_curve[time]++;
            detailed_epi_curve.push_back( make_pair(time, node) );

            vector<Node*> neighbors = node->get_neighbors();
            for (unsigned int i = 0; i<neighbors.size(); i++) {
                if (neighbors[i]->get_state() == 0) {
                    int t = rand_nonuniform_int(time_dist, mtrand) + 1;
                    if (t > infectious_period) {
                        continue;
                    } else {
                        transmissionQ.push( Event( neighbors[i], time + t, node ) );
                    }
                }
            }
        }

        void step_simulation () {
            if (update_time_dist == true) define_time_dist();
            // States: 0 (default) is susceptible
            //         1 is infectious day 1
            //         2 is infectious day 2
            //         ... up to the infectious period
            //         -1 is recovered
            int inf_size = infected.size();
            assert(inf_size > 0);
            time++;
            list<Node*>::iterator inode;
            for (inode=infected.begin(); inode!=infected.end(); inode++) {
                // Increment node states through the infectious period
                (*inode)->set_state( (*inode)->get_state()+1 );
            }

            // Some nodes may be reaching the end of their infectious period ...

            while (inf_size-- > 0) {
                Node* first = infected.front();
                if (first->get_state() <= infectious_period) break;

                first->set_state(-1); // -> recovered
                recovered.push_back( first );
                infected.pop_front();
            }

            while (not transmissionQ.empty() and transmissionQ.top().time <= time) {
                Event event = transmissionQ.top();
                transmissionQ.pop();
                infect_node( event.sink_node );
            }
        }

        void run_simulation() {
            assert(infectious_period > 0 && T >= 0 && T <= 1);
            if (update_time_dist == true) define_time_dist();
            
            // As long as someone's still infected, step simulation
            while (infected.size() > 0)  step_simulation();
        }

        void add_event( Node* sink_node, int time, Node* source_node) {
            transmissionQ.push( Event(sink_node,time,source_node) );
            return;
        }


        int count_infected() {
            return infected.size();
        }

        int epidemic_size() {
            return recovered.size();
        }

        vector< int > get_epi_curve() {
            return epi_curve;
        }

        vector< int > get_prevalence_curve() {
            int len = epi_curve.size() + infectious_period - 1;
            vector< int > p_curve(len, 0);
            int epi_size = 0;
            for (int i = 0; i < len; i++) {
                if (i < (signed) epi_curve.size()) epi_size += epi_curve[i];
                if (i > infectious_period - 1) epi_size -= epi_curve[i - infectious_period];
                p_curve[i] = epi_size;
            }
            return p_curve;
        }

        vector< pair<int, Node*> > get_detailed_epi_curve() {
            return detailed_epi_curve;
        }

        void reset() {
            reset_time();

            while (! infected.empty()) {
                infected.front()->set_state(0);
                infected.pop_front();
            }

            set_these_nodes_to_state(recovered, 0);
            recovered.clear();
            detailed_epi_curve.clear();
        }

        void summary() {
            cerr << "Network size: " << net->size();
            cerr << "\tTransmissibility: " << T;
            cerr << "\tEpidemic size: " << recovered.size() << "\n\n";

        }
};
#endif
