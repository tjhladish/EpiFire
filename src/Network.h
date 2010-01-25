#ifndef NETWORK_H
#define NETWORK_H

#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <math.h>
#include "Utility.h"
#include <assert.h>
#include "MersenneTwister.h"

using namespace std;

//predeclare classes
class Edge;
class Node;
typedef int stateType;

/******************************************************************************
 * These classes have a natural heirarchy of Network > Node > Edge.  That means
 * you probably should not be doing something with nodes unless they already
 * belong to a network, and you probably shouldn't be handling edges unless
 * they already belong to at least one node (and a network).
 *
 * There are many ways you can constuct a network, but they fall into two
 * categories: automated (and probably randomized) and manual.
 *
 * Start out by getting yourself a network:
 *
 *      Network my_network("name", false);  // "name" is any string you want,
 *                                          // and false means network is
 *                                          // undirected (true = directed).
 *
 * If you want to automate construction:
 *      my_network.populate(10,000);        // Add 10^4 nodes to the network.
 *      my_network.rand_connect_poisson(4); // Connect the nodes using a 
 *                                          // Poisson(4) degree distribution
 *                                          // and get rid of self-loops and
 *                                          // multi-edges (done automatically).
 *
 */      

/***************************************************************************
 * 
 * BEGIN NETWORK CLASS 
 *
 **************************************************************************/

class Network
{
    static int id_counter;      //remains in memory until end of the program
    static MTRand mtrand;       //pointer to a radom number generator
    friend class Node;
    friend class Edge;

    public:

    /***************************************************************************
     * Network Constructor and Destructor
     **************************************************************************/
        Network( string name, bool directed );
        Network( const Network& net);
        ~Network();

        Network* duplicate(); // Return a copy, identical except for the network ID
    
    /***************************************************************************
     * Network operators
     **************************************************************************/
        bool operator==( const Network& n2 ); // Test whether two networks are
                                              // actually the same object


    /***************************************************************************
     * Network Accessor Functions (Simple)
     **************************************************************************/
        inline int              get_id() { return id; }
        inline string           get_name() { return name; }
        inline int              size() { return node_list.size(); } // # of nodes in the network
        inline bool             has_unit_edges() { return unit_edges; } // do all edges have same length or cost
        inline bool             is_directed() {return directed; }
	    inline MTRand*          get_rng() { return &mtrand; } // get a pointer to the random number generator


    /***************************************************************************
     * Network Accessor Functions
     **************************************************************************/
        inline vector<Node*> get_nodes() { return node_list; } // get all nodes
        Node*                get_node(int node_id); // get a particular node
        Node*                get_rand_node(); // get a random node
        vector<Edge*>        get_edges(); // get all edges
        Edge*                get_edge(int id); // get a particular edge

        vector<stateType>    get_node_states(); // get states of all nodes

        // get edges that loop back to the same node, or result in
        // redundant connections between all nodes A and B.  NB: To use
        // this, pass in two empty Edge* vectors; they will be populated
        // with the problematic edges inside the function
        void get_bad_edges(vector<Edge*> &self_loops, vector<Edge*> &multiedges);

        vector<Node*> get_component(Node* node);
        vector<Node*> get_major_component();
        // vector< vector<Node*> > get_components(){};


    /***************************************************************************
     * Network Modifier Functions
     **************************************************************************/
        Node* add_new_node();    //creates new node and adds it to the network
        void populate(int n);    //add "n" new nodes to the network
        void add_node( Node* node ); // add an existing node to the network
        void delete_node( Node* node); // delete a specified node (deleting associated edges)

        // erdos_renyi() and rand_connect_poisson() (should) produce equivalent 
        // networks, although the former will always allow degree 0 nodes, while
        // the later can be modified to use a left-truncated distribution.
        //
        // Erdos-Renyi can only produce Poisson distributed networks, whereas
        // all the rand_connect* functions use an adaptation of the Molloy-Reed
        // algorithm, and therefore can take any discrete, non-negative distribution.
        void erdos_renyi(double lambda);
        void ring_lattice(int k);
        void square_lattice(int R, int C, bool diag); // RxC lattice, including diagonals if diag
        void small_world(double p);
        void rand_connect_poisson(double lambda);
        void rand_connect_powerlaw(double alpha, double kappa);
        void rand_connect_exponential(double lambda);
        //void rand_connect_user(map<int,double>); not implemented

        // User provides arbitrary (normalized!) distribution.  normalize_dist(my_dist)
        // in Utility.h can be used for that.  Each value should be the probability 
        // of the index (index = degree), i.e. if the contents of the vector are
        // (0, 0.13, 0.2, 0.04, 0.63), then the probability of drawing a deviate (=degree)
        // of 0 is 0, of 1 is 13%, of 2 is 20%, and so on.
        void rand_connect_user(vector<double> dist);

        // User provides an explicit degree series, one degree per node. Sum of
        // all degrees should be even, or else it's impossible to connect all nodes
        void rand_connect_explicit(vector<int> deg_series);

        // You probably don't want this, unless you are manually creating stubs for
        // each node.  It is likely easier for you to call one of the other
        // rand_connect* functions that takes either a distribution or 
        // distribution parameters.
        void rand_connect_stubs(vector<Edge*> stubs);

        // Gets rid of self-loops and multi-edges.  This is called automatically
        // by erdos_renyi() and all of the rand_connect* functions.  You only 
        // need it if you are using your own algorithm to connect nodes.
        void lose_loops();

        // Unpopulate the network.
        void clear_nodes();// { for (int i = 0; i < size(); i++) delete node_list[i]; }

        // Completely disconnect network.  Degree of every node goes to zero, but things like
        // the node's id, name, state stay the same.
        void clear_edges();

        // Disconnect network, but don't delete edges--leave them as stubs (edges
        // with starting nodes but no ending nodes).  You'll probably want to 
        // follow a call to disconnect_edges() with rand_connect_stubs() to
        // reconnect those edges in a new, randomized way.
        void disconnect_edges();

        void set_node_states(vector<stateType> &states);

    /***************************************************************************
     * Network Input/Output (including visualization)
     **************************************************************************/
        void read_edgelist(string filename); // read network structure from file
        void write_edgelist(string filename);// write network to file
        void graphviz(string filename); // output a graphviz file
        void dumper(); // print the network object contents in the terminal
        bool validate();
        
        vector<int> get_deg_series ();      // list of degrees, one for each node
        vector<int> get_deg_dist ();     // counts of 0 deg nodes, 1 deg nodes, ...
        vector<double> get_gen_deg_dist (); // dist to sample to draw new degrees (frequencies)
        double mean_deg();  // calculated mean of the degree series
        double transitivity(vector<Node*> node_set);  // measure of clustering of nodes in node_set;
                                                      // if node_set is empty, use all nodes
        double mean_dist();   // mean distANCE between all nodes A and B
        vector< vector<double> > all_distances(); // 2D matrix of distances
        
        inline bool topology_altered() { return _topology_altered; }
        inline void set_topology_altered(bool flag) { _topology_altered = flag; }

    /***************************************************************************
     * Network Properties
     **************************************************************************/
    void gen_deg_series(vector<int> &deg_series);
    vector<int> get_states(); // get the states of all nodes
    vector< vector<int> > get_states_by_degree(); // get the state sequences, indexed by degree

    private:
        int id;                  //unique id for the node
        string name;
        vector<Node*> node_list;
        bool unit_edges;
        bool directed;
        vector<double> gen_deg_dist;  // the generating distribution
                                      // used to draw random degrees

        bool _topology_altered;      //has the network topology changed?
        int node_id_counter;
        int edge_id_counter;
        void _assign_deg_series();

        
        // The network has no stubs, but gen_deg_dist (a normalized degree distribution
        // to draw deviates from) has already been stored.
        void _rand_connect();
 
};

class Node
{
    friend class Network;
    friend class Edge;

    public:
    
    /***************************************************************************
     * Constructor and Destructor
     **************************************************************************/
        //Use Network::add_new_node() to add nodes
        void delete_node();
        
        void set_network( Network* network );

        inline int get_id() { return id; }
        inline string get_name() {  return name; }
        inline Network* get_network() { return network; }
        inline vector<Edge*> get_edges_in() { return edges_in; }
        inline vector<Edge*> get_edges_out() { return edges_out; } 
        inline vector<double> get_loc() { return loc; } 
        inline stateType get_state()   { return state; }

        inline void set_loc(const vector<double>& newloc) { this->loc = newloc; }
        inline void set_state(stateType s) { this->state = s; }

        double mean_min_path();
        vector<double> min_paths();
        
        void add_stubs(int deg);

        Edge* get_rand_edge(); // get a random outbound edge
        vector<Node*> get_neighbors ();
        bool is_neighbor(Node* node2);
        void connect_to (Node* end); // a->connect_to(b) == b->connect_to(a) for undirected networks
        bool operator==( const Node& n2 );
        friend ostream& operator<< (ostream &out, Node* node);
        void dumper();

        double min_path(Node* dest);

        Edge* add_stub_out();
        string get_name_or_id();
        int deg();
        
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
};

class Edge
{
    friend class Network;
    friend class Node;

    public:
    /***************************************************************************
     * Constructor and Destructor
     **************************************************************************/
        ~Edge();
        void delete_edge(); //destroys edge (A to B), leaves complement (B to A)
        void disconnect_nodes(); //destroys edge & its complement
        
        inline int get_id() { return id; };
        inline int get_cost() { return cost; };
        inline Node* get_start() { return start; };
        inline Node* get_end() { return end; };
        inline Network* get_network() {return network; };

        void set_cost(int c);
        
        Edge* get_complement();
        void swap_ends (Edge* other_edge);
        void break_end ();
        void define_end (Node* end_node);
        bool is_stub();
        bool operator==( const Edge& e2 );
        friend ostream& operator<< (ostream &out, Edge* edge);
        void dumper();

    private:
        Edge(Node* start , Node* end);

        int id;
        int cost;
        Node* start;
        Node* end;
        Network* network;

};
#endif

