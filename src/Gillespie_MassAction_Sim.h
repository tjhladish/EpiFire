#ifndef GILMASSIM_H
#define GILMASSIM_H

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <queue>
#include "Utility.h"
#include "Network.h"

using namespace std;

class Event {
    public:
        double time;
        char type;
        Event(const Event& o) {  time = o.time; type=o.type; }
        Event(double t, char e) { time=t; type=e; }
        Event& operator=(const Event& o) { time = o.time; type=o.type; }
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

class Gillespie_MassAction_Sim {
    public:
                                    // constructor
        Gillespie_MassAction_Sim( int n, double gamma, double beta) { N=n; GAMMA=gamma; BETA=beta; reset(); }

        int N;                      // population size
        double GAMMA;               // param for exponential recovery time
        double BETA;                // param for exponential transmission time

                                    // event queue
        priority_queue<Event, vector<Event>, compTime > EventQ;
        vector<int> Compartments;   // S, I, R compartments, with counts for each
        //vector<float> Transmissions;
        double Now;                 // Current "time" in simulation

        MTRand mtrand;              // RNG

        void run_simulation() {
//            int day = -1;
            while (next_event()) {
//                if ((int) Now > day) {
//                    cout << Now << "\t" << Compartments[1] << endl;
//                    day = (int) Now;
//                }

                continue;
            }
        }

        int epidemic_size() {
            return Compartments[2]; // Recovered class
        }

        int reset() {
            Now = 0.0;

            Compartments.clear();
            Compartments.resize(3,0);
            Compartments[0] = N;
            
            EventQ = priority_queue<Event, vector<Event>, compTime > ();
            //Transmissions.clear();
        }

        void rand_infect(int k) {   // randomly infect k people
            for (int i = 0; i < k; i++) {
                infect();
            }
            return;
        }

        void infect() {
            assert(Compartments[0] > 0);
            Compartments[0]--;      // decrement susceptibles
            Compartments[1]++;      // increment infecteds
                                    // time to recovery
            double Tr = rand_exp(GAMMA, &mtrand) + Now;
                                    // time to next contact
            double Tc = rand_exp(BETA, &mtrand) + Now;
            while ( Tc < Tr ) {     // does contact occur before recovery?
                add_event(Tc, 'c'); // potential transmission event
                Tc += rand_exp(BETA, &mtrand);
            }
            add_event(Tr, 'r' );
            //Transmissions.push_back(Now);
            return;
        }

        bool is_susceptible(int x) {
            if (Compartments[0] >= x) return true;
            else return false;
        }

        int next_event() {
            if ( EventQ.empty() ) return 0;
            Event event = EventQ.top(); // get the element
            EventQ.pop();               // remove from Q

            Now = event.time;           // advance time
            if (event.type == 'r') {    // recovery event
                Compartments[1]--;      // decrement Infected class
                Compartments[2]++;      // increment Recovered class
            } else {                    // event type must be 'c'
                                 
                // N-2 because person can't self-infect, and because randint includes endpoints
                int contact = mtrand.randInt(N-2) + 1; // add 1 b/c there's no person 0
                if ( is_susceptible(contact) ) infect();

            }
            return 1;
        }

        void add_event( double time, char type) {
            EventQ.push( Event(time,type) );
            return;
        }
/*
        int count_recent_events(double time, double window) {
            int ct = 0;
            for (int i = (signed) Transmissions.size() - 1; i != -1; i--) {
                if (Transmissions[i] > time - window) {
                    ct++;
                } else {
                    break;
                }
            }
            return ct;
        }
*/

};
#endif
