#ifndef NETWORK_H
#define NETWORK_H

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

using namespace std;

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

class Edge {
    
    friend class Network;
    friend class Node;

    public:
        /***************************************************************************
         * Constructor and Destructor
         **************************************************************************/
        ~Edge();
        void delete_edge();      //destroys edge (A to B), leaves complement (B to A)
        void disconnect_nodes(); //destroys edge & its complement

        inline int get_id() const { return id; };
        inline double get_cost() const { return cost; };
        inline Node* get_start() const { return start; };
        inline Node* get_end() const { return end; };
        inline Network* get_network() const {return network; };

        void set_cost(double c);

        Edge* get_complement();
        void swap_ends (Edge* other_edge);
        void break_end ();
        void define_end (Node* end_node);
        bool is_stub();
        bool operator==( const Edge& e2 );
        friend ostream& operator<< (ostream &out, Edge* edge);
        void dumper() const;

    private:
        Edge(Node* start , Node* end);
        void _move_edge(Node* new_start_node);

        int id;
        double cost;
        Node* start;
        Node* end;
        Network* network;

};
#endif
