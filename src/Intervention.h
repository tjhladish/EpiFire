#ifndef INTERVENTION_H
#define INTERVENTION_H

#include "Network.h"

using namespace std;

class Trigger;
class Response;

class Intervention {
    Intervention(Network* network);
    Network* network;
    Queue <Event*> eventQ; 

    bool triggered(int epi_size, double epi_frac, double time);
    Trigger* next_trigger();
    void activate_response();
}

class Event {
    Event();
    Event(Trigger* t, Response* r);
    ~Event();
    Trigger* get_trigger;
    Response* get_response;
    bool has_occurred();
    
    Trigger* trigger;
    Response* response;
    
}

class Trigger {
    enum Type {TIME, EPI_SIZE};
    bool is_satisfied(int epi_size, double epi_frac, double time);
    vector<Response*> responses;
}

class Reponse {
    enum Type {NODE, EDGE, TRANS};
    enum Freq {ABSOLUTE, RELATIVE};
    double val;
    void activate();
}

