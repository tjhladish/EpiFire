#ifndef MASSIM_H
#define MASSIM_H

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <queue>
#include "Utility.h"
#include "Network.h"

using namespace std;

class Event {

    public:

        int patient;
        double time;
        char type;
        Event(const Event& o) {  patient=o.patient; time = o.time; type=o.type; }
        Event(int p,double t, char e) {  patient=p; time=t; type=e; }
        Event& operator=(const Event& o) { patient=o.patient; time = o.time; type=o.type; }

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

class MassAction_Sim {

    public:
        //constructo
        MassAction_Sim( int n, double gamma, double beta) { N=n; GAMMA=gamma; BETA=beta; reset(); }

        int N;                   // population siz
        double p0;               // fraction of population starting each epidemic
        double GAMMA;            // param for exponential recovery time
        double BETA;             // param for exponential transmission time

                                 // event queue
        priority_queue<Event, vector<Event>, compTime > EventQ;
        vector<int> States;      // list of states; 0 is "never been infected"
        vector<int> Rec;         // list of recovery times
        double Now;
        int epi_size;            //size of epidemic

        MTRand mtrand;           // = MTRand();

        void run_simulation() {
            while (next_event()) continue;
        }

        int epidemic_size() {
            // count the number of recovered individuals at the end of the epidemic
            int epi_size = 0;
            for (int i = 0; i<States.size(); i++) {
                if (States[i] == -1) epi_size++;
            }
            return epi_size;
        }

        int reset() {
            Now = 0.0;

            States.clear();
            States.resize(N,0);  // list of states; 0 is "never been infected"

            Rec.clear();
            Rec.resize(N,NULL);  // list of recovery times
        }

        vector<int> rand_infect(int k) {
            vector<int> p_zeros(k);

            rand_nchoosek(N, p_zeros, &mtrand);
            for (int i = 0; i < p_zeros.size(); i++) {
                infect(p_zeros[i]);
            }
            return p_zeros;
        }

        void infect(int x) {     //x is both index and an individual
            States[x] = -2;
            double Tr = rand_exp(GAMMA, &mtrand) + Now;
            double Tc = rand_exp(BETA, &mtrand) + Now;
            if ( Tc < Tr ) add_event(x, Tc, 'c');
            add_event(x,Tr, 'r' );
            Rec[x] = Tr;
            return;
        }

        double susceptibility(int x) {
            if (States[x] < 0) return 0.0;
            else if (States[x] == 0) return 1.0;
        }

        int next_event() {
            if ( EventQ.empty() ) return 0;
                                 //get the element
            Event event = EventQ.top();
            EventQ.pop();        //remove from Q

            Now = event.time;
            int patient = event.patient;
            if (event.type == 'r') {
                                 // -1 is 'recovered'
                States[patient] = -1;
                Rec[patient] = NULL;
            }                    // event type must be 'c'
            else {
                                 // N-2 because person can't self-infect, and because randint includes endpoints
                int contact = mtrand.randInt(N-2);
                                 // this way we never draw the patient himself
                if (contact >= patient) contact++;

                double s = susceptibility(contact);
                if (s == 1) infect(contact);
                else if (s > 0) {
                    if (mtrand.rand() < s ) infect(contact);
                }

                // now let's see if patient will infect again
                                 // time when recovery will occur
                double Tr = Rec[patient];
                                 // time of next potential contact
                double Tc = rand_exp(BETA, &mtrand) + Now;
                                 // add event if contact will happen before recovery
                if (Tc < Tr) add_event(patient, Tc, 'c');
            }
            //    delete event;
            return 1;
        }

        void add_event( int patient, double time, char type) {
            EventQ.push( Event(patient,time,type) );
            return;
        }

};
#endif
