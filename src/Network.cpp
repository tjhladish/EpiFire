#include "Network.h"
#include "Utility.h"

////////////////////////////////////////////////////////////////////////////////
//
// Global Class Variables
//
////////////////////////////////////////////////////////////////////////////////

int Network::id_counter = 0;
MTRand Network::mtrand;          // single instance of random number generator for entire program run
// the rng can be seeded at any time by calling mtrand->seed(your_seed);

////////////////////////////////////////////////////////////////////////////////
//
// Network Class Functions
//
////////////////////////////////////////////////////////////////////////////////

Network::Network( string name, bool directed) {
    id = Network::id_counter++;
    this->name = name;
    this->directed = directed;
    this->unit_edges = 1;
    this->node_id_counter = 0;
    this->edge_id_counter = 0;
    this->_topology_altered=false;
    this->mtrand = mtrand;
    this->process_stopped = false;
    this->progress = -1;
}


Network* Network::duplicate() {
    Network* dup = new Network( name, directed );
    dup->unit_edges = unit_edges;
    dup->node_id_counter = node_id_counter;
    dup->edge_id_counter = edge_id_counter;
    dup->mtrand = mtrand;

    // Make copies of all nodes
    for (int i = 0; i < size(); i++) {
        Node* node = node_list[i];
        Node* node_copy = new Node();
        node_copy->id   = node->get_id();
        node_copy->set_network(dup);

        // Create a stub for each outbound edge (since not all nodes have been created, we can't connect everything yet)
        node_copy->add_stubs( node->deg() );
        dup->node_list.push_back(node_copy);
    }

    // Loop through nodes again, this time defining edges
    for (int i = 0; i < size(); i++) {
        Node* node = node_list[i];
        Node* node_copy = dup->get_node( node->get_id() );

        vector<Edge*> edges = node->get_edges_out();
        vector<Edge*> edge_copies = node_copy->get_edges_out();

        for (unsigned int j = 0; j < edges.size(); j++) {
            edge_copies[j]->id    = edges[j]->get_id();
            edge_copies[j]->cost  = edges[j]->get_cost();
            edge_copies[j]->end   = edges[j]->get_end();
        }
    }
    return dup;
}


Network::~Network() {
    for (unsigned int i = 0; i < node_list.size(); i++) {
        delete node_list[i];
    }
    //delete mtrand;
}


void Network::populate( int n ) {
    for (int i = 0; i < n; i++) {
        add_new_node();
    }
}


Node* Network::add_new_node() {
    Node* node = new Node();     //allocate memory for new node
    node->id = node_id_counter++;
    node->set_network(this);     //set the network
    node_list.push_back(node);   //add new node to the back
    set_topology_altered(true);
    return(node);
}


void Network::delete_node(Node* node) {
    vector<Node*>::iterator itr;
    itr = find(node_list.begin(), node_list.end(), node);
    node_list.erase(itr);
    delete this;
    set_topology_altered(true);
}


Node* Network::get_node(int node_id) {
    assert((unsigned) node_id < node_list.size() );

    Node* lucky_node = node_list[node_id];
    if (lucky_node->id == node_id) {
        return lucky_node;
    }

    vector<Node*>::iterator itr;
    for (itr = node_list.begin(); itr < node_list.end(); itr++) {
        if ((*itr)->id == node_id) return *itr;
    }
    cerr << "Couldn't find a node with id  " << node_id << endl;
    return NULL;
}


bool Network::ring_lattice(int k) {
    if (k > (size() - 1) / 2) {
        cerr << "Cannot construct a ring lattice with k-nearest neighbors where k > (network size - 1) / 2\n";
        return false;
    }
    for (unsigned int i = 0; i < node_list.size(); i++) {
        for (int j = 1; j <= k; j++) {
            int dest = (i+j) % node_list.size();
            node_list[i]->connect_to(node_list[dest]);
        }
    }
    return true;
}


// Assumes undirected network
bool Network::square_lattice(int R, int C, bool diag) {
    if (R < 1 or C < 1) {
        cerr << "Square lattice must have at least one row and one column.\n";
        return false;
    }
    clear_nodes();
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            Node* node = add_new_node();
            int id = node->get_id();
                                 // if we're not in the first row or column
            if (i > 0 && j > 0) {
                int up = id - C; // connect to node above
                Node * up_node = get_node(up);
                node->connect_to( up_node );
                int left = id - 1;
                Node * left_node = get_node(left);
                                 // & connect to node on left
                node->connect_to( left_node );
                if (diag) {
                    node->connect_to( get_node(up-1) );
                    left_node->connect_to( up_node );
                }
            }
            else if (i > 0) {
                int up = id - C;
                node->connect_to( get_node(up) );
            }                    // we must be in the first row
            else if (j > 0) {
                int left = id - 1;
                node->connect_to( get_node(left) );
            }
        }
    }
    return true;
}


//void Network::small_world(double p) {}

// generates a poisson network vi the Erdos & Renyi algorithm
bool Network::erdos_renyi(double lambda) {
    int n = size();
    if (lambda > n-1) return false; // mean degree can't be bigger than network size - 1 
    double p = lambda / (n-1);
    vector<Node*> nodes = get_nodes();
    for (int a = 0; a < n - 1; a++) {
        if (is_stopped() ) { return false; }
        for (unsigned int b = a; b < nodes.size(); b++) {
            if ( mtrand.rand() < p) {
                nodes[a]->connect_to(nodes[b]);
            }
        }
        //set_progress( (double) a / n );
        PROG( (int) 100 * (1 - (double)(n-a)*(n-a-1) / (n*(n-1))) );
        //cerr <<  1 - (double)(n-a)*(n-a-1) / (n*(n-1))  << endl;
    }
    return true;
}


// generates a poisson network.  Faster than Erdos-Renyi for sparse graphs
bool Network::sparse_random_graph(double lambda) {
    int n = size();
    double p = lambda / (n-1);
    double sd = sqrt(n*(n-1)*p*(1-p));
                                 // randNorm(mean, variance)
    double edge_ct = mtrand.randNorm(lambda * n, sd);
                                 // we're increasing the degree of 2 nodes!
    for (int i = 0; i < edge_ct; i += 2) {
        int a = mtrand.randInt(n - 1);
        int b = mtrand.randInt(n - 1);
                                 // for undirected graphs, this makes
        node_list[a]->connect_to(node_list[b]);
                                 // an undirected edge
    }
    return lose_loops();
}

bool Network::fast_random_graph(double lambda) {
    // Testing networks of 10e3, 10e4, and 10e5 nodes, I found that the break-even point
    // where the algorithms performance was comparable was when nodes were connected to
    // between 1 and 2% of the network
    if (lambda/size() > 0.01) {
        return erdos_renyi(lambda);
    } else {
        return sparse_random_graph(lambda);
    }
}

bool Network::rand_connect_poisson(double lambda) {
    int min = 0;                 // min and max are INCLUSIVE, i.e. the lowest and highest possible degrees
    int max = node_list.size() - 1;
    vector<double> dist = gen_trunc_poisson(lambda, min, max);
    return rand_connect_user(dist);
}


bool Network::rand_connect_powerlaw(double alpha, double kappa) {
    int min = 1;                 // min and max are INCLUSIVE, i.e. the lowest and highest possible degrees
    int max = node_list.size() - 1;
    vector<double> dist = gen_trunc_powerlaw(alpha, kappa, min, max);
    return rand_connect_user(dist);
}


bool Network::rand_connect_exponential(double lambda) {
    int min = 1;                 // min and max are INCLUSIVE, i.e. the lowest and highest possible degrees
    int max = node_list.size() - 1;
    vector<double> dist = gen_trunc_exponential(lambda, min, max);
    return rand_connect_user(dist);
}


/*
// This might be used if the degree distribution had only very large values or was otherwise very weird
void Network::rand_connect_user(map<int,double>) {

}*/

bool Network::rand_connect_explicit(vector<int> degree_series) {
    assert(degree_series.size() == node_list.size());
    assert(sum(degree_series) % 2 == 0);
    for (unsigned int i = 0; i < degree_series.size(); i++ ) {
        node_list[i]->add_stubs(degree_series[i]);
    }
    return rand_connect_stubs( get_edges() );
}


bool Network::rand_connect_user(vector<double> dist) {
    gen_deg_dist = dist;
    _assign_deg_series();
    return rand_connect_stubs( get_edges() );
}


// use this only if the generating degree dist has already been stored
bool Network::_rand_connect() {
    _assign_deg_series();
    return rand_connect_stubs( get_edges() );
}


// rand_connect_stubs() expects ONLY stubs in network, i.e. not some complete edges
// and some stubs
bool Network::rand_connect_stubs(vector<Edge*> stubs) {
                                 //get all edges in network
    vector<Edge*>::iterator itr;
    Edge* m;
    Edge* n;

    //shuffle the vector
    //int max = stubs.size() - 1;
    //for (int i = max; i >= 0; i-- ) swap(stubs[i], stubs[ mtrand.randInt(i) ]);

    shuffle(stubs, &mtrand);

    //connect stubs

    //int stupid_var = stubs.size() - 1;
    for (unsigned int i = 0; i < stubs.size() - 1; i += 2 ) {
        m  = stubs[i];
        n  = stubs[i  + 1];
        m->define_end(n->start);
        n->define_end(m->start);
    }
    // if lose_loops() isn't successful, return false
    if (! lose_loops()) { clear_edges(); return false; }
    return true;
}


// This method removes self loops (edges with the same node as start and end)
// and multi-edges (e.g. pairs of edges which have identical starts and ends)
// Returns true on success, false if network could not be rewired
bool Network::lose_loops() {
    if ( is_stopped() ) return false;
                                 //all (outbound) edges in the network
    vector<Edge*> edges = get_edges();
    vector<Edge*>::iterator edge1, edge2;

    int m, n;
    int failed_attempts = 0;

    Node* start1;
    Node* start2;
    Node* end1;
    Node* end2;

    vector<Edge*> self_loops, multiedges;

    get_bad_edges( self_loops, multiedges);
    vector<Edge*> bad_edges;

    bad_edges.insert(bad_edges.begin(), self_loops.begin(), self_loops.end());
    bad_edges.insert(bad_edges.end(), multiedges.begin(), multiedges.end());
    //cerr << "Bad edge count: " << bad_edges.size() << endl;

    //shuffle the vector
    int max = bad_edges.size() - 1;
    for (int i = max; i >= 0; i-- ) swap(bad_edges[i], bad_edges[ mtrand.randInt(i) ]);

    while ( bad_edges.size() > 0 ) {
        m = bad_edges.size() - 1;
        n = mtrand.randInt(  edges.size() - 1 );
        if ( failed_attempts > 99 ) {
            cerr    << "It may be impossible to equilibriate a network with these parameters--"
                << "couldn't get rid of any self-loops or multi-edges in the last 100 attempts"
                << endl;
            return false;
        }
        if ( is_stopped() ) return false;

        Edge* edge1 = bad_edges[m];
        Edge* edge2 = edges[n];

        start1 = edge1->start;
        end1   = edge1->end;
        start2 = edge2->start;
        end2   = edge2->end;
        /*
        Rule 1: End of edge 1 must not equal start of edge 2, and vice versa.  At worst, this prevents synonymous changes, and at best, it prevents some self-loops and multiedges.

        Rule 2: Start of edge 1 must not equal start of edge 2, and end of edge 1 must not equal end of edge 2.  This prevents synonymous (i.e. non-structural, edge re-numbering) changes.

        Rule 3: End of edge 1 must not have an edge that goes to start of edge 2, and vice versa.  This may be expensive to enforce because it requires looking around two nodes for every proposed change. It prevents some multiedges.
        */
        if ( end1 == start2  || end2 == start1 || start1 == start2 || end1 == end2 ) {
            failed_attempts++;
            continue;
        }

        // Rule 3
        vector<Node*> neighbors1 = end1->get_neighbors();
        vector<Node*> neighbors2 = end2->get_neighbors();
        vector<Node*>::iterator result1, result2;
        result1 = find( neighbors1.begin(), neighbors1.end(), start2 );
        result2 = find( neighbors2.begin(), neighbors2.end(), start1 );

        if( result1 != neighbors1.end() || result2 != neighbors2.end() ) {
            //            cerr << "Broke Rule 3 " << endl ;
            failed_attempts++;
            continue;
        }

        Edge* edge1_comp = edge1->get_complement();
        bad_edges.pop_back();

        for ( unsigned int i = 0; i < bad_edges.size(); i++ ) {
            if ( bad_edges[i]==edge1_comp ) {
                bad_edges.erase(bad_edges.begin() + i);
                //                cerr << "spliced out complement " << endl;
                break;
            }
        }

        //        cerr << "swapping edges: " << edge1->id  << " " << edge2->id << endl;
        failed_attempts = 0;
        edge1->swap_ends(edge2);
    }

    //    self_loops.clear();
    //    multiedges.clear();
    //    get_bad_edges( self_loops, multiedges);
    return true;
}


void Network::get_bad_edges(vector<Edge*> &self_loops, vector<Edge*> &multiedges) {
    vector<Edge*> edges = get_edges();

    map< int, map <int, int> > seen_edges;

    Edge* edge;
    Node* start;
    Node* end;
    for(unsigned int i=0; i < edges.size(); i++ ) {
        edge = edges[i];
        start = edge->start;
        end = edge->end;
        if ( start == end ) {
            self_loops.push_back( edge );
        }
        else {
            if ( seen_edges[ start->id ][ end->id ] > 0  ) {
                multiedges.push_back(edge);
            }
            seen_edges[ start->id ][ end->id ]++;
        }
    }

    //cerr <<  "get_bad_edges() " << seen_edges.size() <<  " " << self_loops.size() <<  " " << multiedges.size() << endl;
}


vector<Node*> Network::get_biggest_component() {
    vector<Node*> big_comp(0);
    vector< vector<Node*> > all_comp = get_components();

    for (unsigned int i = 0; i<all_comp.size(); i++) {
        if (all_comp[i].size() > big_comp.size()) big_comp = all_comp[i];
    }
/*    vector<int> remaining_nodes(size(),1);

    while ((unsigned) sum(remaining_nodes) > major_comp.size()) {
        Node* starting_point = NULL;
        for ( unsigned int i = 0; i < remaining_nodes.size(); i++ ) {
            if (remaining_nodes[i] == 1) {
                starting_point = get_node(i);
                break;
            }
        }

        vector<Node*> temp_comp = get_component(starting_point);
        for ( unsigned int i = 0; i < temp_comp.size(); i++ ) remaining_nodes[i] = 0;
        if (temp_comp.size() > major_comp.size()) major_comp = temp_comp;
    }*/
    return big_comp;
}


vector< vector<Node*> > Network::get_components() {
    vector< vector<Node*> > components;
    vector<Node*> temp_comp(0);
    list<int> remaining_nodes;
    for (int i = 0; i<size(); i++) remaining_nodes.push_back( get_nodes()[i]->get_id() );

    while ( remaining_nodes.size() > 0) {
        Node* next = get_node( remaining_nodes.front() );
        temp_comp = get_component( next );
        components.push_back(temp_comp);

        for (unsigned int i = 0; i<temp_comp.size(); i++) remaining_nodes.remove( temp_comp[i]->get_id() );
    }
    return components;
}


vector<Node*> Network::get_component(Node* node) {
    vector<Node*> hot_nodes;
    vector<Node*> cold_nodes;
    map<int,int> state;          // 1 means "hot", 2 means "cold", 0 means we haven't looked at it yet

    hot_nodes.push_back(node);
    state[node->id] = 1;

    while (hot_nodes.size() > 0) {
        vector<Node*> new_hot_nodes;
        for (unsigned int i = 0; i < hot_nodes.size(); i++) {
            vector<Node*> neighbors = hot_nodes[i]->get_neighbors();
            for (unsigned int j = 0; j < neighbors.size(); j++) {
                                 // maybe you've already looked at this node
                if ( state[neighbors[j]->id] > 0 ) continue;
                state[neighbors[j]->id] = 1;
                new_hot_nodes.push_back( neighbors[j] );
            }
            state[hot_nodes[i]->id] = 2;
            cold_nodes.push_back( hot_nodes[i] );
        }
        hot_nodes = new_hot_nodes;
    }
    return cold_nodes;
}


void Network::_assign_deg_series() {
    int n = this->node_list.size();
    vector<int> deg_series(n);

    gen_deg_series(deg_series);

    for (int i = 0; i < n; i++ ) {
        this->node_list[i]->add_stubs(deg_series[i]);
    }
}


void Network::gen_deg_series(vector<int> &deg_series) {
    for (unsigned int i = 0; i < deg_series.size(); i++ ) {
        deg_series[i] = rand_nonuniform_int(gen_deg_dist, &mtrand);
    }

    while ( sum(deg_series) % 2 == 1 ) {
        int idx = mtrand.randInt( deg_series.size() - 1 );
        deg_series[idx] = rand_nonuniform_int(gen_deg_dist, &mtrand);
    }
}


vector<int> Network::get_deg_series () {
    vector<int> deg_series( size() );
    for (int i = 0; i < size(); i++) deg_series[i] = node_list[i]->deg();
    return deg_series;
}


vector<int> Network::get_deg_dist () {
    vector<int> series = get_deg_series();
    vector<int> deg_dist(max_element(series) + 1);
    for (unsigned int i = 0; i < series.size(); i++) deg_dist[ series[i] ]++;
    return deg_dist;
}


vector<double> Network::get_gen_deg_dist () {
    // generating (idealized) degree distribution, if one exists
    if (gen_deg_dist.size() == 0) {
        vector<int> deg_dist = get_deg_dist();
        int Sum = sum(deg_dist);
        gen_deg_dist = normalize_dist(deg_dist, Sum);
    }
    return gen_deg_dist;
}


double Network::mean_deg () {
    return mean(get_deg_series() );
}


vector<int> Network::get_states() {
    vector<int> states(size());
    vector<Node*> nodes = get_nodes();
    for (unsigned int i = 0; i < nodes.size(); i++) {
        states[i] = nodes[i]->get_state();
    }
    return states;
}


// For each degree, get the series of states
// E.g. states_by_degree[4] would be the vector of states for nodes w/ deg=4
vector< vector<int> > Network::get_states_by_degree() {
    vector<int> deg_dist = get_deg_dist();
    vector< vector<int> > states_by_degree(deg_dist.size());
    for (unsigned int i=0; i<states_by_degree.size(); i++ ) {
        vector<int> states(deg_dist[i], 0);
        states_by_degree[i] = states;
    }
    vector<int> counter(deg_dist.size(), 0);
    vector<Node*> nodes = get_nodes();
    for ( unsigned int i = 0; i < nodes.size(); i++ ) {
        int deg   = nodes[i]->deg();
        states_by_degree[deg][counter[deg]] = nodes[i]->get_state();
        counter[deg]++;
    }
    return states_by_degree;
}


double Network::transitivity (vector<Node*> node_set) {
    if (node_set.size() == 0) node_set = node_list;
    int triangles = 0;
    int tripples  = 0;
    Node *a, *b, *c;

    for (unsigned int i = 0; i < node_list.size(); i++) {
        a = node_list[i];
        vector<Node*> neighborhood_a = a->get_neighbors();
        for (unsigned int j = 0; j < neighborhood_a.size(); j++) {
            b = neighborhood_a[j];
            vector<Node*> neighborhood_b = b->get_neighbors();
            for (unsigned int k = 0; k < neighborhood_b.size(); k++) {
                c = neighborhood_b[k];
                if ( c == a ) continue;
                if ( c->is_neighbor(a) ) triangles++;
                tripples++;
            }
        }
    }
    return (double) triangles / (double) tripples ;
}


double Network::mean_dist(vector<Node*> node_set) {    // average distance between nodes in network
    if (node_set.size() == 0) node_set = node_list;
    vector< vector<double> > distance_matrix = calculate_distances(node_set);
    double grand_total = 0;
    for( unsigned int i=0; i < distance_matrix.size(); i++ ) {
        grand_total += sum(distance_matrix[i]);
    }
    double mean = grand_total / ( size()*( size()-1 ) ); // don't consider distance from nodes to themselves
    return mean;
}

// if node_set is not provided, default is all nodes.  node_set would generally be
// all nodes within a single component
vector< vector<double> > Network::calculate_distances(vector<Node*> node_set) {
    if (node_set.size() == 0) node_set = node_list;
    vector< vector<double> > dist( node_set.size() );
    for(unsigned int i = 0; i < node_set.size(); i++ ) {
        dist[i] = node_set[i]->min_paths(node_set);
    }
    return dist;
}


Edge* Network::get_edge(int edge_id) {
    for (unsigned int i=0; i< node_list.size(); i++ ) {
        vector<Edge*> edges = node_list[i]->edges_out;
        for(unsigned int j=0; j < edges.size(); j++ ) {
            if ( edges[j]->id == edge_id ) {
                return(edges[j]);
            }
        }
    }
    cerr << "Coundn't find an edge with id " << edge_id << endl;
    return NULL;
}


vector<Edge*> Network::get_edges() {
    vector<Edge*> all_edges;
    for (unsigned int i=0; i< node_list.size(); i++ ) {
        vector<Edge*> edges = node_list[i]->edges_out;
        for(unsigned int j=0; j < edges.size(); j++ ) {
            all_edges.push_back(edges[j]);
        }
    }
    return(all_edges);
}


Node* Network::get_rand_node() {
    int max = node_list.size() - 1;
    return node_list[ mtrand.randInt(max) ];
}


// Unpopulate the network.
void Network::clear_nodes() {
    for (int i = 0; i < size(); i++) delete node_list[i];
    node_list.clear();
    set_topology_altered(true);
}


// Completely disconnect network.  Degree of every node goes to zero, but things like
// the node's id, name, state stay the same.
void Network::clear_edges() {
    for (int i = 0; i < size(); i++) {
        vector<Edge*> edges = node_list[i]->edges_out;
        for (unsigned int j = 0; j < edges.size(); j++ ) delete edges[j];
        node_list[i]->edges_out.clear();
        node_list[i]->edges_in.clear();
    }
    set_topology_altered(true);
}


// Disconnect network, but don't delete edges--leave them as stubs (edges
// with starting nodes but no ending nodes).  You'll probably want to
// follow a call to disconnect_edges() with rand_connect_stubs() to
// reconnect those edges in a new, randomized way.
void Network::disconnect_edges() {
    for (int i = 0; i < size(); i++) {
        vector<Edge*> edges = node_list[i]->edges_out;
        for (unsigned int j=0; j< edges.size(); j++ ) edges[j]->break_end();
    }
    set_topology_altered(true);
}


void Network::set_node_states(vector<stateType> &states) {
    if ((signed) states.size() != size()) {
        cerr << "Error in Network::set_node_states(): Vector of node states has size " << states.size() << " but there are " << size() << " nodes in network.\n";
    }
    else {
        for (int i = 0; i < size() ; i++) {
            node_list[i]->set_state( states[i] );
        }
    }
}


vector<stateType> Network::get_node_states() {
    vector<stateType> states;
    for (int i = 0; i < size() ; i++) {
        states.push_back( node_list[i]->get_state() );
    }
    return states;
}


void Network::dumper() {
    cerr << "Dumping network:\n";
    cerr << "name => " << name << "\n";
    cerr << "id => " << id << "\n";
    cerr << "n => " << node_list.size() << endl;
    cerr << "directed => " << directed  << endl;
    cerr << "nodes => \n";
    vector <Node*>::iterator itr;
    for ( itr = node_list.begin(); itr != node_list.end(); itr++ ) (*itr)->dumper();
}


bool Network::validate() {
    int net_error = 0;
    for (int i = 0; i < size(); i++) {
        Node* node = node_list[i];
        if (node == NULL) { cerr << "node with index " << i << " is undefined (NULL)" << endl; net_error = true; continue;}
        vector<Edge*> in = node->edges_in;
        vector<Edge*> out = node->edges_out;
        if (in.size() != out.size() && directed == false) cerr << "node" << node << ": Number of edges in does not match number out: " << in.size() << ", " << out.size() << endl;
        map<int, int> seen;
        int error_ct = 0;
        for (unsigned int j = 0; j < in.size(); j++) {
            Edge* edge = in[j];
            Node* start = edge->start;
            Node* end = edge->end;
            if (edge->is_stub())  {cerr << "node" << node << ": Found edge in edges_in vector that has NULL endpoint: edge" << edge << endl; error_ct++;}
            if (start == NULL)  {cerr << "node" << node << ": Found edge in edges_in vector that has NULL startpoint: edge" << edge << endl; error_ct++;}
            if (end != NULL && end->id != node->id) {cerr << "node" << node << ": Found edge in edges_in vector that does not end at this node: edge" << edge << ", end: " << end << endl; error_ct++;}
            if (start != NULL && end != NULL && end == start) {cerr << "node" << node << ": Found self-loop in edges_in vector: edge" << edge << endl; error_ct++;}
            if (start != NULL && seen[start->id]++ != 0) {cerr <<  "node" << node << ": Found " << seen[start->id]  << " multi-edges from node" << start << endl; error_ct++;}
        }
        for (unsigned int j = 0; j < out.size(); j++) {
            Edge* edge = out[j];
            Node* start = edge->start;
            Node* end = edge->end;
            if (start == NULL)  {cerr << "node" << node << ": Found edge in edges_out vector that has NULL startpoint: edge" << edge << endl; error_ct++; continue;}
            if (start->id != node->id) {cerr << "node" << node << ": Found edge in edges_out vector that does not start at this node: edge" << edge << ", start: " << start << endl; error_ct++;}
            if (edge->is_stub())  {cerr << "node" << node << ": Found stub in edges_out vector that has NULL endpoint: edge" << edge << endl; error_ct++;}
            if (start != NULL && end != NULL && end == start) {cerr << "node" << node << ": Found self-loop in edges_in vector: edge" << edge << endl; error_ct++;}
            if (end != NULL && --seen[end->id] < 0) {cerr <<  "node" << node << ": Found edge in edges_out vector that is not a complement of an edge in edges_in vector: edge" << edge << endl; error_ct++;}
        }
        if (error_ct > 0) {
            node->dumper(); cerr << "node" << node << ": Found " << error_ct << " error(s).\n\n";
            net_error += error_ct;
        }
    }
    if ( net_error == 0) {
        cerr << "Network PASSED validation.\n";
        return true;
    }
    cerr << "Network FAILED validation.\n";
    return false;
}


// read_edgelist currently supports only undirected networks
void Network::read_edgelist(string filename) {

    cerr << "Loading " << filename << endl;
    ifstream myfile(filename.c_str());
    std::stringstream ss;
    map<string,Node*> idmap;

    if (myfile.is_open()) {
        string line;
        while ( getline(myfile,line) ) {
            //split string based on "," and store results into vector
            vector<string>fields;
            split(line,',', fields);

            //format check
            if (fields.size() != 2 ) {
                cerr << "problem with line " << line << endl;
                continue;
            }

            const char whitespace[] = " \n\t\r";
            string name1 = strip(fields[0],whitespace);
            string name2 = strip(fields[1],whitespace);

            cerr << line << endl;
            if(idmap.count(name1)) cerr << name1 << " " << idmap[name1] << endl ;
            if(idmap.count(name2)) cerr << name2 << " " << idmap[name2] << endl ;
            cerr << "---" << endl;

                                 //new node;
            if(idmap.count(name1)==0) {
                                 //allocate memory for new node
                Node* node = this->add_new_node();
                node->name = name1;
                idmap[name1] = node;
            }

                                 //new node;
            if(idmap.count(name2)==0) {
                                 //allocate memory for new node
                Node* node = this->add_new_node();
                node->name = name2;
                idmap[name2]=node;
            }

            Node *n1 = idmap[name1];
            Node *n2 = idmap[name2];
            n1->connect_to(n2);
        }
    }
}


void Network::write_edgelist(string filename) {
    if (filename == "") filename = "edgelist.out";

    ofstream pipe(filename.c_str(), ios::out);
    vector<Edge*> edges = get_edges();
    for (unsigned int i = 0; i < edges.size(); i++) {
        int start_id = edges[i]->start->id;
        int end_id   = edges[i]->end->id;
        if (start_id > end_id) continue;
        pipe << start_id << "," << end_id << endl;
    }
    pipe.close();

}


void Network::graphviz (string filename) {
    /*    if (get_edges().size() > 200) {
            cerr << "Network is too large (> 200 edges) to reasonably output with graphviz/DOT\n";
            return;
        }*/
    if (filename == "") filename = "tmp.dot";

    ofstream pipe(filename.c_str(), ios::out);

    string graph_type = directed ? "digraph" : "graph";

    map< int, map <int, int> > seen_edges;
    pipe << graph_type << " NETWORK {\n\t\trankdir=LR;\n";
    cout << graph_type << " NETWORK {\n\t\trankdir=LR;\n";

    vector<Node*>::iterator node_it;
    node_it = node_list.begin();

    while (node_it != node_list.end() ) {

        int start = (*node_it)->id;

        string start_name = to_string( (*node_it)->get_name_or_id() );
        pipe << start << " [label=\"" << start_name << "\"]\n";
        cout << start << " [label=\"" << start_name << "\"]\n";

        if ( (*node_it)->deg() == 0 ) {
            cerr << "Encountered unconnected node when trying to draw network.  This is currently not supported for visualization: node will be ignored.\n";
            continue;
        }
        vector<Edge*>::iterator edge_it;
        edge_it = (*node_it)->edges_out.begin();

        while (edge_it != (*node_it)->edges_out.end() ) {

            int end = (*edge_it)->end->id;

            string end_name = to_string( (*edge_it)->end->get_name_or_id() );

            if ( seen_edges[end][start] > 0 ) {
                seen_edges[end][start]--;
                seen_edges[start][end]--;
                edge_it++;
                continue;
            }
            else {
                seen_edges[start][end]++;
            }
            string edge_op = directed ? " -> " : " -- ";
            int edge_id = (*edge_it)->id;

            cout << "\t\t" << start << edge_op << end << " [label=\"" << edge_id << "\"];\n";
            pipe << "\t\t" << start << edge_op << end << " [label=\"" << edge_id << "\"];\n";
            edge_it++;
        }
        node_it++;
    }

    cout << "}\n";
    pipe << "}\n";

    pipe.close();
}

bool Network::is_stopped() {
    bool status = process_stopped;
    process_stopped = false;
    return status;
}


////////////////////////////////////////////////////////////////////////////////
//
// Node Class Functions
//
///////////////////////////////////////////////////////////////////////////////

Node::Node(Network* network, string name, stateType state) {
    this->network = network;
    this->id = network->node_id_counter++;
    this->name = name;
    this->state = state;
}


Node::Node() {                   //empty constructor
    this->network = NULL;
    this->id = -1;
    this->name= "";
    this->state = 0;
}


Node::~Node() {                  //destructor
    //cerr << "~Node() " << id << endl;
    for(unsigned int i=0; i< edges_out.size(); i++ ) delete edges_out[i];
    edges_out.clear();
}


void Node::set_network(Network* network) {
    this->network = network;
}


void Node::add_stubs(int deg) {
    for (int i = 0; i < deg; i++) add_stub_out();
}


Edge* Node::add_stub_out () {
    Edge* stub = new Edge(this,NULL);
    edges_out.push_back(stub);
    network->set_topology_altered(true);
    return stub;
}


string Node::get_name_or_id () {
    return (name != "") ? name : to_string(id);
}


int Node::deg () {
    return edges_out.size();
}


Edge* Node::get_rand_edge() {
    assert(deg() > 0);
    return edges_out[ network->mtrand.randInt( deg() - 1 ) ];
}


vector<Node*> Node::get_neighbors () {
    vector<Node*> neighbors;
    for (unsigned int i = 0; i < edges_out.size(); i++) {
        neighbors.push_back(edges_out[i]->end);
    }
    return neighbors;
}


bool Node::is_neighbor (Node* node2) {
    vector<Node*> neighbors = get_neighbors();
    for (unsigned int i = 0; i < neighbors.size(); i++) if (neighbors[i] == node2) return true;
    return false;
}


void Node::connect_to (Node* end) {
    Edge* edge1 = add_stub_out();
    edge1->define_end(end);
    if ( ! network->is_directed()) {
        Edge* edge2 = end->add_stub_out();
        edge2->define_end(this);
    }
}


void Node::_add_inbound_edge (Edge* edge) {
    edges_in.push_back(edge);
    network->set_topology_altered(true);
}


//this doesn't delete the edge object, it merely disconnects it from the node that it was going to.
void Node::_del_inbound_edge (Edge* inbound) {
    if (! inbound->end->id == this->id ) {
        cerr << "The 'inbound' edge does not connect to the node provided." << endl;
        exit(1);
    }
    vector<Edge*>::iterator itr = find(edges_in.begin(), edges_in.end(), inbound);
    edges_in.erase(itr);
}


bool Node::operator==( const Node& n2 ) {
    if ( id == n2.id) return true;
    return false;
}


ostream& operator<< (ostream &out, Node* node) {
    out << node->id;
    return out;
}


void Node::dumper() {

    cerr << "\tname => " << name << endl;
    cerr << "\tid => "<< id << endl;
    cerr << "\tdegree => " << deg() <<  endl;
    cerr << "\tlocation => ";
    copy( loc.begin(), loc.end(), ostream_iterator<double>(cerr, " "));
    cerr << endl;

    vector<Edge*>::iterator itr;

    cerr << "\tedges out => \n";
    for(itr = edges_out.begin(); itr != edges_out.end(); itr++ ) (*itr)->dumper();

    cerr << "\tedges in => \n";
    for(itr = edges_in.begin();  itr != edges_in.end();  itr++ ) (*itr)->dumper();

    cerr << "\n";
}

// Mean path length from this node to all others in same component
double Node::mean_min_path() {
    int component_size = 0;
    vector<Node*> empty;
    vector<double> distances = min_paths(empty);
    double sum = 0;
    for (int i = 0; i < (signed) distances.size(); i++) {
        if (distances[i] > -1 && id != i) {
            component_size++;
            sum += distances[i];
        }
    }
    double mean = (double) sum / (double) component_size;
                                 //quantum computing!! NAN != NAN is true
    if (mean != mean) cerr << "Mean_minimum_path is not meaningful for one-node components.\n" << endl;
    return mean;
}


double Node::min_path(Node* dest) {
    vector<Node*> empty(0);
    vector<double> distances = this->min_paths(empty);
    return distances[dest->id];
}


// Calculates length of the minimum path (if possible) between *this* and everything in *nodes*
// If *nodes* is empty, default is all nodes
vector<double> Node::min_paths(vector<Node*> nodes) {
    if (nodes.size() == 0) nodes = get_network()->node_list;
    map <Node*, double> known_cost; //Per Dijkstra's Algorithm, these are the two lists
    map <Node*, double>::iterator itr;
                                 //we need to keep track of
    map <Node*, double> uncertain_cost;

                                 //the following initializes the 'uncertain' set with undefined values,
                                 //since we have no information about these nodes yet.
    for (unsigned int i = 0; i < nodes.size(); i++) {
        if (this == nodes[i]) continue;
                                 // infinity =: 10^99
        uncertain_cost[ nodes[i] ] = 1e+99;
    }
    known_cost[this] = 0;        //We only know initially that there is no cost to get to the starting node

    int j = 0;
                                 //As long as there are nodes with uncertain min costs
    while ( j++ < (signed) nodes.size() ) {
                                 //Loop through the nodes we know about.
                                 //Initialize min to an arbitrary node in the "uncertain" map
        Node* min = NULL;        //(*uncertain_cost.begin()).first;
        for ( itr = known_cost.begin(); itr != known_cost.end(); itr++) {
                                 //key of the map is the node
            Node* known_node = (*itr).first;
                                 //Get the outbound edges for this known node

            vector <Edge*> edges = known_node->edges_out;
            for (unsigned int i = 0; i < edges.size(); i++) {
                                 //Get this neighbor
                Node* end = edges[i]->end;
                                 //Move on if this endpoint already has a known cost
                if ( known_cost.count(end) > 0 ) continue;
                                 //Otherwise, calculate a cost using this path
                int cost = known_cost[known_node] + edges[i]->cost;

                //Store the new cost as an uncertain cost
                //if it's better than the others we've seen
                if (uncertain_cost.count(end) > 0 && uncertain_cost[end] < cost) {
                    continue;
                }
                else {

                    if ( min == NULL ) min = end;
                    uncertain_cost[end] = cost;
                    if ( uncertain_cost[end] < uncertain_cost[min] ) min = end;
                }
            }
        }

        //Now that we've looked at all the nodes we can get to from the known nodes,
        //find the least-cost uncertain node, and move that to the known group.
        //The two conditionals testing for undefs make sure that undefs are ranked
        //as higher costs than all numeric values
        int size = known_cost.size();

        if ( min != NULL) {
            known_cost[min] = (int) uncertain_cost[min];
            //            uncertain_cost.erase(min);
        }
        if ((unsigned) size == known_cost.size()) break;
    }                            //test  this->id = start;

    int n = nodes.size();
    vector<double> distances(n,-1);
    for ( int i=0; i<n; i++) {
        if ( known_cost.count(nodes[i]) ) {
            distances[i] = known_cost[nodes[i]];
        }
    }
    return distances;
}


////////////////////////////////////////////////////////////////////////////////
//
// Edge Functions
//
////////////////////////////////////////////////////////////////////////////////

Edge::Edge(Node* start, Node* end) {
    this->network = start->network;
    this->id = this->network->edge_id_counter++;
    this->start = start;
    this->end = end;
    this->cost = 1;
}


Edge::~Edge() {  /*cerr << "removing edge " << id << endl;*/ }

void Edge::delete_edge() {
    vector<Edge*>::iterator itr;

    if (end != NULL) {
        itr = find(end->edges_in.begin(), end->edges_in.end(), this);
        end->edges_in.erase(itr);
    }

    itr = find(start->edges_out.begin(), start->edges_out.end(), this);
    start->edges_out.erase(itr);

    network->set_topology_altered(true);
    delete this;
}


void Edge::disconnect_nodes() {
    Edge* comp = get_complement();

    delete_edge();
    comp->delete_edge();
}


void Edge::set_cost(int c) {
    this->cost=c;
}


Edge* Edge::get_complement () {
                                 // get the edges leaving the endpoint;
    vector<Edge*> edges = end->edges_out;
                                 // one is bound to be the complement
    for (unsigned int i = 0; i < edges.size(); i++) {
        //    cerr << edges[i]->start->id << " " << edges[i]->end->id << endl;
        if (edges[i]==this) continue;
        if (edges[i]->end==start) return edges[i];
    }
    cerr << "Failed to find a complement (antiparallel edge) to the edge provided.  There may be a problem with the network structure." << endl;
    exit(1);
}


void Edge::swap_ends (Edge* other_edge) {

    Edge* this_comp = get_complement();
    Edge* other_comp = other_edge->get_complement();

    break_end();
    define_end(other_comp->start);
    this_comp->break_end();
    this_comp->define_end(other_edge->start);
    other_edge->break_end();
    other_edge->define_end(this_comp->start);
    other_comp->break_end();
    other_comp->define_end(start);
}


/*
    sub swap_ends {
        my ( $self, $other_edge ) = @_;
        my @edges = ( $self, $self->get_complement(), $other_edge, $other_edge->get_complement() );
        my @rev_edges = reverse @edges;
        for my $i ( 0 .. 3 ) {
            my $edge = $edges[$i];
            $edge->break_end();
            $edge->define_end( $rev_edges[$i]->get_start() );
        }
        return;
    }
*/

void Edge::break_end () {
    end->_del_inbound_edge(this);
    end = NULL;
    network->set_topology_altered(true);
}


void Edge::define_end (Node* end_node) {
    end = end_node;
    end->_add_inbound_edge(this);
    network->set_topology_altered(true);
}


bool Edge::is_stub() {
    if (end == NULL) return true;
    return false;
}


bool Edge::operator==( const Edge& e2 ) {
    if ( id == e2.id) return true;
    return false;
}


ostream& operator<< (ostream &out, Edge* edge) {
    out << edge->id;
    return out;
}


void Edge::dumper () {
    string space = "\t\t";
    string start_name = start->get_name_or_id();
    string end_name = ( end != NULL ) ? end->get_name_or_id() : "undef";
    cerr << space << "id => " << id << ", start => " << start_name << ", end => " << end_name << endl;
}
