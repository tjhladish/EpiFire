#ifndef EF_NODE_H
#define EF_NODE_H

#include <iterator>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <random>
#include <limits>

#include <EpiFire/Utility.h>
#include <EpiFire/Shared.h>
#include <EpiFire/Network.h>
#include <EpiFire/Edge.h>

using namespace std;

class Node {
    friend class Network;
    friend class Edge;

    public:

        inline bool is_stopped() {return network->is_stopped();}

        /***************************************************************************
         * Constructor and Destructor
         **************************************************************************/
        //Use Network::add_new_node() to add nodes
        void delete_node();

        void set_network( Network* network );

        inline int get_id() const { return id; }
        inline string get_name() {  return name; }
        inline Network* get_network() const { return network; }
        inline vector<Edge*> get_edges_in() const { return edges_in; }
        inline vector<Edge*> get_edges_out() const { return edges_out; }
        inline vector<double> get_loc() const { return loc; }
        inline stateType get_state() const { return state; }

        inline void set_loc(const vector<double>& newloc) { this->loc = newloc; }
        inline void set_state(stateType s) { this->state = s; }

        double mean_min_path();

        // for path length calculations, infinite distances == -1
        DistanceMatrix min_path_map(vector<Node*>& node_set) const;
        DistanceMatrix min_path_map() const {vector<Node*> tmp; return min_path_map(tmp);}
        vector<double> min_paths(vector<Node*>& node_set) const;
        vector<double> min_paths() const {vector<Node*> tmp; return min_paths(tmp);}

        void add_stubs(int deg);

        Edge* get_rand_edge();   // get a random outbound edge
        vector<Node*> get_neighbors () const;
        bool is_neighbor(Node* node2) const;
                                 // a->connect_to(b) == b->connect_to(a) for undirected networks
        void connect_to (Node* end);
        bool disconnect_from (Node* end); // true if they were connected, false if not
        bool change_neighbors(Node* old_neighbor, Node* new_neighbor);
        bool operator==( const Node& n2 );
        friend ostream& operator<< (ostream &out, Node* node);
        void dumper() const;

        double min_path(Node* dest);

        Edge* add_stub_out();
        string get_name_or_id();
        int deg() const;

    private:
        Node();
        Node(Network* network, string name, stateType state);
        ~Node();

        int id;                  //unique id
        string name;
        Network* network;        //pointer to network
        vector<Edge*> edges_in;  //vector of pointers coming in
        vector<Edge*> edges_out; //vector of pointers going out
        vector<double> loc;
        stateType state;
        void _add_inbound_edge (Edge* edge);
        void _del_inbound_edge (Edge* inbound);
        void _add_outbound_edge (Edge* edge);
        void _del_outbound_edge (Edge* outbound);

        DistanceMatrix _min_paths(vector<Node*>& node_set) const; // infinite distances == -1
        DistanceMatrix _min_unweighted_paths(vector<Node*>& node_set) const; // infinite distances == -1
};

#endif // EF_NODE_H
