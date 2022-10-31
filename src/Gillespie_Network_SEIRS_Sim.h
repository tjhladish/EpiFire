#ifndef GIL_NET_SEIRS_SIM_H
#define GIL_NET_SEIRS_SIM_H

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <queue>
#include "Utility.h"
#include "Network.h"
#include <climits>

using namespace std;

class Event {
    public:
        double time;
        char type;
        Node* node;
        Event(const Event& o) {  time=o.time; type=o.type; node=o.node; }
        Event(double t, char e, Node* n) { time=t; type=e; node=n; }
        Event& operator=(const Event& o) { time=o.time; type=o.type; node=o.node; return *this; }
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


class Gillespie_Network_SEIRS_Sim {
    public:
        // S -> E -> I -> R -> S
        typedef enum {
            SUSCEPTIBLE, EXPOSED, INFECTIOUS, RESISTANT, STATE_SIZE // STATE_SIZE must be last
        } stateType;
                                    // constructor
        Gillespie_Network_SEIRS_Sim ( Network* net, double m, double b, double g, double im_dur, double sde, double sdt) {
            network = net;
            mu = m;
            beta = b;
            gamma = g;
            immunity_duration = im_dur; // Immunity duration is fixed (not exponentially distributed)
            social_distancing_effect = sde;
            social_distancing_threshold = sdt;
            detected_cases = 0;
            social_distancing_start_date = INT_MAX;
            reset();
        }

        Network* network;           // population
        double mu;                  // param for exponential exposed duration
        double beta;                // param for exponential time to transmission
        double gamma;               // param for exponential time to recovery
        double immunity_duration;   // duration of recovered/resistant state before becoming susceptible
        size_t detected_cases;
        double social_distancing_effect;
        size_t social_distancing_threshold;
        int social_distancing_start_date;

                                    // event queue
        priority_queue<Event, vector<Event>, compTime > EventQ;
        vector<int> state_counts;   // S, E, I, R counts
        double Now;                 // Current "time" in simulation

        mt19937 rng;              // RNG

        void run_simulation(double duration, int serial) {
            double start_time = Now;
            int day = (int) Now;
            while (next_event() and Now < start_time + duration) {
                if ((int) Now > day) {
                    cout << serial << "\t" << (int) Now << "\t"  << state_counts[SUSCEPTIBLE] << "\t" 
                                          << state_counts[EXPOSED] << "\t" 
                                          << state_counts[INFECTIOUS] << "\t" 
                                          << state_counts[RESISTANT] << "\t"
                                          << detected_cases;
                    if (social_distancing_start_date < Now) cout << " *"; 
                                          cout << endl; 
                    day = (int) Now;
                }

                continue;
            }
        }

        int current_epidemic_size() {
            return state_counts[EXPOSED] + state_counts[INFECTIOUS];
        }

        void reset() {
            Now = 0.0;
           
            vector<Node*> nodes = network->get_nodes();
            for (unsigned int i = 0; i < network->size(); i++) nodes[i]->set_state(SUSCEPTIBLE);

            state_counts.clear();
            state_counts.resize(STATE_SIZE, 0);
            state_counts[SUSCEPTIBLE] = network->size();
            
            EventQ = priority_queue<Event, vector<Event>, compTime > ();
        }

        // choose n nodes without replacement
        vector<Node*> rand_choose_nodes (int n) {
            assert(n > -1 and n <= network->size());
            vector<Node*> nodes = network->get_nodes();
            vector<Node*> sample(n);
            vector<int> sample_ids(n);
            rand_nchoosek(network->size(), sample_ids, &rng);
            Node* node;
            for (unsigned int i = 0; i < sample_ids.size(); i++) {
                node = nodes[ sample_ids[i] ];
                sample[i] = node;
            };
            return sample;
        }


        void rand_infect(int n) {   // randomly infect k people
            vector<Node*> sample = rand_choose_nodes(n);
            for (unsigned int i = 0; i < sample.size(); i++) {
                infect(sample[i]);
            }
            return;
        }

        void infect(Node* node) {
            assert(state_counts[SUSCEPTIBLE] > 0);
            node->set_state(EXPOSED);
            state_counts[SUSCEPTIBLE]--;  // decrement susceptible groupjj
            state_counts[EXPOSED]++;      // increment exposed group

                                    // time to become infectious
            double Ti = rand_exp(mu, &rng) + Now;
            add_event(Ti, 'i', node);
                                    // time to recovery
            double Tr = rand_exp(gamma, &rng) + Ti;
                                    // time to next contact
            //const double beta_sd = Now > social_distancing_timing ? beta*(1.0 - social_distancing_effect) : beta;

            if (detected_cases > social_distancing_threshold and social_distancing_start_date == INT_MAX) {
                social_distancing_start_date = Now + 10; // 10 days for symptoms + testing
            }
            const double beta_sd = social_distancing_start_date < Now ? beta*(1.0 - social_distancing_effect) : beta;
            double Tc = rand_exp(beta_sd, &rng) + Ti;
            while ( Tc < Tr ) {     // does contact occur before recovery?
                add_event(Tc, 'c', node); // potential transmission event
                Tc += rand_exp(beta_sd, &rng);
            }
            add_event(Tr, 'r', node);
                                    // time to become susceptible again
            double Ts = Tr + immunity_duration; 
            add_event(Ts, 's', node);
            return;
        }

        int next_event() {
            if ( EventQ.empty() ) return 0;
            Event event = EventQ.top(); // get the element
            EventQ.pop();               // remove from Q

            Now = event.time;           // advance time
            Node* node = event.node;
            if (event.type == 'i') { 
const double DETECT_PROB = 0.013; // based on FL numbers
if (rand_uniform(0, 1, &rng) < DETECT_PROB) detected_cases++;
                node->set_state(INFECTIOUS); 
                state_counts[EXPOSED]--;      // decrement Infected class
                state_counts[INFECTIOUS]++;   // increment Recovered class
            } else if (event.type == 'r') {   // recovery event
                node->set_state(RESISTANT);
                state_counts[INFECTIOUS]--;   // decrement Infected class
                state_counts[RESISTANT]++;    // increment Recovered class
            } else if (event.type == 's') {   // loss of immunity event
                node->set_state(SUSCEPTIBLE);
                state_counts[RESISTANT]--;
                state_counts[SUSCEPTIBLE]++;
            } else if (event.type == 'c') {                          // event type must be 'c'
                                 
                vector<Node*> neighbors = node->get_neighbors();
                if (neighbors.size() > 0) {
                    int rand_idx = rand_uniform_int(0, neighbors.size() - 1, &rng);
                    Node* contact = neighbors[rand_idx];
                    if ( contact->get_state() == SUSCEPTIBLE ) infect(contact);
                }
            } else {
                cerr << "Unknown event type encountered in simulator: " << event.type << "\nQuitting.\n";
            }
            return 1;
        }

        void add_event( double time, char type, Node* node) {
            EventQ.push( Event(time,type,node) );
            return;
        }

};
#endif
