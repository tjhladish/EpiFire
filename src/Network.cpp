#include "Network.h"
#include "Utility.h"
#include <array>
#include <functional>

////////////////////////////////////////////////////////////////////////////////
//
// Global Class Variables
//
////////////////////////////////////////////////////////////////////////////////

int Network::id_counter = 0;
std::mt19937 Network::rng;          // single instance of random number generator for entire program run
// the rng can be seeded at any time by calling rng.seed(your_seed) where your_seed is of type uint32_t

////////////////////////////////////////////////////////////////////////////////
//
// Network Class Functions
//
////////////////////////////////////////////////////////////////////////////////

Network::Network( string name, netType directed) {
    id = Network::id_counter++;
    this->name = name;
    this->directed = directed;
    this->unit_edges = 1;
    this->node_id_counter = 0;
    this->edge_id_counter = 0;
    this->_topology_altered=false;
    this->process_stopped = false;
    this->known_nodes = 0;
}


Network::Network(netType directed) : Network("", directed) { }


Network* Network::duplicate() const {
    Network* dup = new Network( name, directed );
    dup->unit_edges         = unit_edges;
    dup->node_id_counter    = node_id_counter;
    dup->edge_id_counter    = edge_id_counter;
    dup->_topology_altered  = _topology_altered;
    dup->process_stopped    = process_stopped;
    dup->known_nodes        = known_nodes;
    dup->gen_deg_dist       = gen_deg_dist;

    // Make copies of all nodes
    for (int i = 0; i < size(); i++) {
        Node* node = node_list[i];
        Node* node_copy = new Node();
        node_copy->id    = node->get_id();
        node_copy->name  = node->get_name();
        node_copy->loc   = node->get_loc();
        node_copy->state = node->get_state();
        node_copy->set_network(dup);

        // Create a stub for each outbound edge
        // (since not all nodes have been created, we can't connect everything yet)
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
            edge_copies[j]->define_end( dup->get_node( edges[j]->get_end()->get_id() ) );
        }
    }
    return dup;
}


Network::~Network() {
    for (unsigned int i = 0; i < node_list.size(); i++) {
        Node* n = node_list[i];

        for (unsigned int e = 0; e < n->edges_out.size(); e++) delete n->edges_out[e];
        n->edges_out.clear();

        n->edges_in.clear();

        delete n;
    }
}

void Network::seed() {
    std::random_device rd; //supposedly has fallbacks; look into replacing with a more robust solution
    std::array<std::mt19937::result_type, std::mt19937::state_size> seed_data;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(rd));
    std::seed_seq seed(std::begin(seed_data), std::end(seed_data)); //rng takes a seed_seq to fill bits initially
    rng.seed(seed);
}

void Network::seed(std::uint32_t seed) {
    rng.seed(seed);
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


Node* Network::add_new_node(string name, stateType state) {
    Node* node = add_new_node();
    node->name = name;
    node->state = state;
    return(node);
}


void Network::delete_node(Node* node) {
    node->delete_node();
}


void Network::reset_node_ids() {
    for (unsigned int i = 0; i < node_list.size(); ++i) node_list[i]->id = i;
}


Node* Network::get_node(int node_id) {
    if ((unsigned) node_id < node_list.size() and node_list[node_id]->id == node_id) {
        return node_list[node_id];
    }

    vector<Node*>::iterator itr;
    for (itr = node_list.begin(); itr < node_list.end(); itr++) {
        if ((*itr)->id == node_id) return *itr;
    }
    cerr << "Couldn't find a node with id  " << node_id << endl;
    return NULL;
}


Node* Network::get_node_by_name(string node_name) {
    vector<Node*>::iterator itr;
    for (itr = node_list.begin(); itr < node_list.end(); itr++) {
        if ((*itr)->name == node_name) return *itr;
    }
    cerr << "Couldn't find a node with name  " << node_name << endl;
    return NULL;
}


bool Network::is_weighted() {
    vector<Edge*> edges = get_edges();
    for( unsigned int i = 0; i<edges.size(); i++) {
        if (edges[i]->get_cost() != 1) {
            return true;
        }
    }
    return false;
}


bool Network::ring_lattice(int N, int K) {
    if (K > (N - 1) / 2) {
        cerr << "Cannot construct a ring lattice with K-nearest neighbors where K > (network size - 1) / 2\n";
        return false;
    }
    clear_nodes();
    populate(N);
    for (unsigned int i = 0; i < node_list.size(); i++) {
        for (int j = 1; j <= K; j++) {
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


bool Network::small_world(int N, int K, double beta) {
    if ( ring_lattice(N, K) ) {
        vector<Node*> nodes = get_nodes();
        for (int i=0; i<size(); i++) {
            Node* node = nodes[i];
            int degree = node->deg();
            // How many of node's edges will be shuffled?
            int m = rand_binomial( degree, beta, &rng );
            // Which edges will be shuffled?
            vector<int> edge_indeces(m);
            rand_nchoosek( degree, edge_indeces, &rng );
            
            vector<Edge*> edges = node->get_edges_out();
            for (unsigned int e=0; e<edge_indeces.size(); e++) {
                // Get the current neighbor associated with this edge
                Node* neighbor = edges[e]->get_end();

                // Make sure that there are nodes that can be connected to
                int prospective_neighborhood = N - 1 - degree;
                if (prospective_neighborhood > 0) {
                    bool success = false;
                    int attempts = 0;
                    while (! success) {
                        // Grab a node from the network
                        Node* prospective = get_rand_node();
                        // If it's not this node, and not a current neighbor
                        if ( prospective != node and ! node->is_neighbor(prospective) ) {
                            // then connect to it, and ditch the old neighbor
                            success = node->change_neighbors( neighbor, prospective );
                        }
                        if (++attempts > 1000) {
                            // Give up if we've tried to rewire this edge 1000 times
                            cerr << "Failed to find a prospective neighbor after trying 1000 times in small world generator.\n";
                            return false;
                        }
                    }
                }
            }
        }
    } else {
        return false; // making ring lattice failed
    }
    return true;
}

// generates a poisson network vi the Erdos & Renyi algorithm
bool Network::erdos_renyi(double lambda) {
    int n = size();
    if (lambda > n-1) return false; // mean degree can't be bigger than network size - 1
    double p = lambda / (n-1);
    vector<Node*> nodes = get_nodes();
    for (int a = 0; a < n - 1; a++) {
        if (is_stopped() ) { return false; }
        for (unsigned int b = a + 1; b < nodes.size(); b++) {
            if ( rand_uniform(0, 1, &rng) < p) {
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
    long double p = lambda / (n-1);
    long double sd = sqrtl(n*lambda*(1-p));
    //sqrtl(n*(n-1)*p*(1-p)); // sometimes yields -nan (e.g. n=50000,lambda=5)
    double edge_ct = rand_normal(lambda * n, sd, &rng);
                                 // we're increasing the degree of 2 nodes!
    for (int i = 0; i < edge_ct; i += 2) {
        int a = rand_uniform_int(0, n-1, &rng);
        int b = rand_uniform_int(0, n-1, &rng);
                                 // for undirected graphs, this makes
        node_list[a]->connect_to(node_list[b]);
                                 // an undirected edge
        PROG( (int) 50 * i/edge_ct );
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
    if (is_stopped()) return false;
    gen_deg_dist = dist;
    return _rand_connect();
}


// use this only if the generating degree dist has already been stored
bool Network::_rand_connect() {
    if (_assign_deg_series()) {
        return rand_connect_stubs( get_edges() );
    } else {
        return false;
    }
}


bool Network::rand_connect_stubs(vector<Edge*> stubs) {
    if ( is_stopped() ) return false;
    if ( stubs.size() == 0 ) return true;
    assert(stubs.size()%2 == 0);
                                 //get all edges in network
    Edge* m;
    Edge* n;

    //shuffle the vector
    shuffle(stubs, &rng);

    //connect stubs
    for (unsigned int i = 0; i < stubs.size() - 1; i += 2 ) {
        m  = stubs[i];
        n  = stubs[i  + 1];
        m->define_end(n->start);
        n->define_end(m->start);
        PROG( 25 + (int) (25 * i / stubs.size()) );
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
    shuffle(bad_edges, &rng);
    int max = bad_edges.size() - 1;

    while ( bad_edges.size() > 0 ) {
        PROG( 50 + (int) (50 * (max - bad_edges.size()) / max) );
        m = bad_edges.size() - 1;
        n = rand_uniform_int(0, edges.size() - 1, &rng);
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
            assert(start != NULL);
            assert(end != NULL);
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

    return big_comp;
}


vector< vector<Node*> > Network::get_components() {
    vector< vector<Node*> > components;
    vector<Node*> temp_comp(0);
    set<Node*> seen_nodes;
    PROG(0);

    for (unsigned int i=0; i<node_list.size(); i++) {
    	if (is_stopped()) {
    		vector< vector<Node*> > empty;
    	    return empty;
    	}

    	if (seen_nodes.count(node_list[i]) == 0) {
    		temp_comp = get_component(node_list[i]);
    		components.push_back(temp_comp);

    		//for (int j=0; j<temp_comp.size(); j++) seen_nodes.insert(temp_comp[j]);
    		std::copy( temp_comp.begin(), temp_comp.end(), std::inserter( seen_nodes, seen_nodes.end() ) );

    	}
    }

    return components;
}


vector<Node*> Network::get_component(Node* node) {
    vector<Node*> hot_nodes;
    set<Node*> cold_nodes;
    hot_nodes.push_back(node);


    while (hot_nodes.size() > 0) {
        if (process_stopped) {
        	vector<Node*> output(cold_nodes.begin(), cold_nodes.end());
        	return output;
        }

        vector<Node*> new_hot_nodes;
        for (unsigned int i = 0; i < hot_nodes.size(); i++) {

            vector<Node*> neighbors = hot_nodes[i]->get_neighbors();
            for (unsigned int j = 0; j < neighbors.size(); j++) {

                if ( cold_nodes.count(neighbors[j]) != 0 ) continue; // maybe you've already looked at this node
                new_hot_nodes.push_back( neighbors[j] );
            }

            PROG((int) 100*((float) (known_nodes + cold_nodes.size())/size() ));
        }
        std::copy( hot_nodes.begin(), hot_nodes.end(), std::inserter( cold_nodes, cold_nodes.end() ) );
        hot_nodes = new_hot_nodes;
    }
    vector<Node*> output(cold_nodes.begin(), cold_nodes.end());
    return output;
}


bool Network::_assign_deg_series() {
    int n = this->node_list.size();
    vector<int> deg_series(n);

    if ( ! gen_deg_series(deg_series) ||  is_stopped() ) return false;
    int running_sum = 0;
    for (int i = 0; i < n; i++ ) {
        this->node_list[i]->add_stubs(deg_series[i]);
        running_sum += this->node_list[i]->deg();
        PROG( (int) (25.0 * i / n));
    }
    assert(running_sum % 2 == 0);
    return true;
}


bool Network::gen_deg_series(vector<int> &deg_series) {
    double dist_sum = sum(gen_deg_dist);
    if (dist_sum < 1 - 1e-14 || dist_sum > 1 + 1e-14) {
        //cerr << "Sum: " << setprecision(50) << (double) sum(gen_deg_dist) << endl;
        cerr << "Degree distribution does not sum to 1\n";
        return false;
    }

    for (unsigned int i = 0; i < deg_series.size(); i++ ) {
        deg_series[i] = rand_nonuniform_int(gen_deg_dist, &rng);
    }

    while ( sum(deg_series) % 2 == 1 ) {
        int idx = rand_uniform_int(0, deg_series.size() - 1, &rng);
        deg_series[idx] = rand_nonuniform_int(gen_deg_dist, &rng);
    }
    return true;
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


map<Node*, int> Network::k_shell_decomposition() {
    map<Node*, int> ks;
    // Initialize values to the nodes' degrees
    for (int i = 0; i < size(); i++) ks[node_list[i]] = node_list[i]->deg();

    // Nodes that haven't been categorized yet (the high k-shell nodes)
    list<Node*> core(node_list.begin(), node_list.end());
    list<Node*>::iterator itr;
    int current_shell = 0;
    bool hit = false; // Have we categorized any nodes in this pass?
    // As long as there are nodes left to categorize
    while(core.size() > 0) {
        for (itr = core.begin(); itr !=core.end(); ) { // Do not increment itr here!
            if (ks[*itr] == current_shell) {
                // This node has a ks equal to the current_shell
                vector<Edge*> edges_out = (*itr)->edges_out;
                for (unsigned int i = 0; i < edges_out.size(); i++) {
                    // Decrement the tentative k-shell for this node's neighbors ...
                    Node* neighbor = edges_out[i]->end;
                    // ... but only if they're in the core
                    if (ks[neighbor] > current_shell) ks[neighbor]--;
                }
                itr = core.erase(itr); // increment itr after erasing
                hit = true;
            } else {
                ++itr;                 // increment itr without erasing
            }
        }
        if (hit == false) current_shell++;
        hit = false;
    }
    return ks;
}


/*
// Algorithmically correct, simpler than k_shell_decomposition,
// but slower, too.  Used it to validate the faster algorithm.
map<string, int> Network::k_shell_decomposition_alt() {
    map<string, int> ks;
    Network* tmp = this->duplicate();
    bool hit = false;
    int current_shell = 0;
    vector<Node*> to_delete;
    while(tmp->size() > 0) {
        for (int i = 0; i < tmp->size(); i++) {
            if (tmp->node_list[i]->deg() <= current_shell) {
                ks[tmp->node_list[i]->get_name()] = current_shell;
                hit = true;
                to_delete.push_back(tmp->node_list[i]);
            }
        }

        for (unsigned int i = 0; i<to_delete.size(); i++) {
            to_delete[i]->delete_node();
        }
        to_delete.clear();

        if (hit == false) current_shell++;
        hit = false;
    }
    delete tmp;
    return ks;
}
*/


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
        if (is_stopped()) return -1 * std::numeric_limits<float>::max();
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
        PROG(100*i/size());
    }
    return (double) triangles / (double) tripples ;
}


double Network::mean_dist(vector<Node*> node_set) {    // average distance between nodes in network
    if (node_set.size() == 0) node_set = node_list;
    vector< vector<double> > distance_matrix;
    calculate_distances(node_set, distance_matrix);

    double grand_total = 0.0;
    int ct = 0;
    for( unsigned int i=0; i < distance_matrix.size(); i++ ) {
        for( unsigned int j=0; j < distance_matrix[i].size(); j++ ) {
            grand_total += distance_matrix[i][j];
            ct++;
        }
    }
    double mean = grand_total / ct;
    return mean;
}


// if node_set is not provided, default is all nodes.  node_set would generally be
// all nodes within a single component
// Assumes undirected network
void Network::calculate_distances(vector<Node*>& full_node_set, vector< vector<double> >& dist)  {
    if (full_node_set.size() == 0) full_node_set = node_list;
    for(unsigned int i = 0; i < full_node_set.size() - 1; i++ ) {
        vector<Node*> node_set;
        for(unsigned int j = i+1; j < full_node_set.size(); j++ ) {
            node_set.push_back(full_node_set[j]);
        }

        vector<double> empty;
        dist.push_back(empty);

        if (is_stopped() ) {
            return;
        }

        PROG(100*(i-1)/node_set.size());
        dist[i] = full_node_set[i]->min_paths(node_set);
    }
    return;
}


PairwiseDistanceMatrix Network::calculate_distances_map() {
    PairwiseDistanceMatrix dist_map;
    if (is_directed()) {
        for (Node* n: node_list) dist_map[n] = n->min_path_map();
    } else {
        PairwiseDistanceMatrix tmp_dist_map;
        for (unsigned int i=0; i<node_list.size(); ++i){
            const Node* n = node_list[i];
            vector<Node*> node_subset = vector<Node*>(node_list.begin()+i, node_list.end());
            tmp_dist_map[n] = n->min_path_map(node_subset);
        }

        for (pair<const Node*, DistanceMatrix> pair_ndm: tmp_dist_map) {
            const Node* n = pair_ndm.first;
            if (dist_map.count(n) == 0) dist_map[n] = DistanceMatrix();
            for (pair<const Node*, double> pair_nd: pair_ndm.second) {
                const Node* m  = pair_nd.first;
                if (dist_map.count(m) == 0) dist_map[m] = DistanceMatrix();
                const double d = pair_nd.second;
                dist_map[n][m] = d;
                dist_map[m][n] = d;
            }
        }
    }
    return dist_map;
}


// if node_set is not provided, default is all nodes.  node_set would generally be
// all nodes within a single component
// Assumes undirected network
void Network::print_distances(vector<Node*>& full_node_set)  {
    vector< vector<double> > dist;
    calculate_distances(full_node_set, dist);

    for(unsigned int i = 0; i < full_node_set.size() - 1; i++ ) {
        for(unsigned int j = i+1; j < full_node_set.size(); j++ ) {
            cout << full_node_set[i]->name << "\t" << full_node_set[j]->name << "\t" << dist[i][j-i-1] << endl;
        }
    }
    return;
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
    return node_list[ rand_uniform_int(0, max, &rng) ];
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


bool Network::shuffle_edges(double frac) {
    vector<Edge*> stubs;
    vector<Edge*> edges = get_edges();
    if (is_directed()) {
        cerr << "Shuffling edges not implemented for directed networks.\n";
        exit(1);
    } else {
        vector< pair <Edge*, Edge*> > edge_pairs;

        for (unsigned int e = 0; e < edges.size(); e++) {
            Edge* edge = edges[e];
            Edge* comp = edge->get_complement();
            if ( edge->id < comp->id ) {
                pair<Edge*, Edge*> edge_pair(edge, comp);
                edge_pairs.push_back(edge_pair);
            }
        }
        int num_pairs_to_shuffle = (int) (frac * edges.size()/2 + 0.5); // rounding instead of truncating
        vector<int> sample(num_pairs_to_shuffle);
        rand_nchoosek((int) edge_pairs.size(), sample, &rng);

        for (unsigned int i = 0; i < sample.size(); i++) {
            Edge* edge1 = edge_pairs[ sample[i] ].first;
            Edge* edge2 = edge_pairs[ sample[i] ].second;
            edge1->break_end();
            edge2->break_end();
            stubs.push_back(edge1);
            stubs.push_back(edge2);
        }
    }
    return rand_connect_stubs( stubs );
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


void Network::dumper() const {
    cerr << "Dumping network:\n";
    cerr << "name => " << name << "\n";
    cerr << "id => " << id << "\n";
    cerr << "n => " << node_list.size() << endl;
    cerr << "directed => " << directed  << endl;
    cerr << "nodes => \n";
    vector <Node*>::const_iterator itr;
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
void Network::read_edgelist(string filename, char sep) {
    ifstream myfile(filename.c_str());
    std::stringstream ss;
    map<string,Node*> idmap;

    if (myfile.is_open()) {
        string line;

        while ( getline(myfile,line) ) {
            //split string based on "," and store results in vector
            vector<string> fields;
            split(line, sep, fields);
            const char whitespace[] = " \n\t\r";

            //format check
            if (fields.size() > 2 ) {
                cerr << "Skipping line: too many fields: " << line << endl;
                continue;
            } else if (fields.size() == 1) { // unconnected node
                Node* node = this->add_new_node();
                string name1 = strip(fields[0],whitespace);
                cerr << "Found single node " << name1 << endl;
                node->name = name1;
                idmap[name1] = node;
                continue;
            } else if (fields.size() < 1) { // empty line
                continue;
            } else { // there are exactly 2 nodes

                string name1 = strip(fields[0],whitespace);
                string name2 = strip(fields[1],whitespace);

                //node1 is new; allocate memory
                if(idmap.count(name1)==0) {
                    Node* node = this->add_new_node();
                    node->name = name1;
                    idmap[name1] = node;
                }

                //node2 is new; allocate memory
                if(idmap.count(name2)==0) {
                    Node* node = this->add_new_node();
                    node->name = name2;
                    idmap[name2]=node;
                }

                idmap[name1]->connect_to(idmap[name2]);
            }
        }
    }
}

bool Network::add_edgelist(ifstream& source, char sep, string breaker) {
    std::stringstream ss;
    const char whitespace[] = " \n\t\r";
    if (source.is_open()) {
        string line;

        while ( getline(source,line) && line.compare(breaker) != 0 ) {
            //split string based on "," and store results in vector
            vector<string> fields;
            split(line, sep, fields);


            //format check
            if (fields.size() > 2 ) {
                cerr << "Skipping line: too many fields: " << line << endl;
                continue;
            } else if (fields.size() == 1) { // unconnected node
                unsigned int id;
                std::istringstream(strip(fields[0],whitespace)) >> id;
                cerr << "Found single node " << id << " which is in idmap: " << (id < node_list.size()) << endl;
                continue;
            } else if (fields.size() < 1) { // empty line
            	cerr << "Found an empty line." << endl;
            	continue;
            } else { // there are exactly 2 nodes

                unsigned int idA;
                std::istringstream(strip(fields[0],whitespace)) >> idA;
                unsigned int idB;
                std::istringstream(strip(fields[1],whitespace)) >> idB;

                if(node_list.size() < idA) {
                	cerr << "Found an id not in network: " << idA << endl;
                	continue;
                }

                if(node_list.size() < idB) {
                	cerr << "Found an id not in network: " << idB << endl;
                	continue;
                }

                node_list[idA]->connect_to(node_list[idB]);
            }
        }
        return breaker.compare(line) == 0;
    } else {
    	return false;
    }
}


void Network::write_edgelist(string filename, outputType ot, char sep) {
    if (filename == "") filename = "edgelist.out";

    ofstream pipe(filename.c_str(), ios::out);
    vector<Edge*> edges;
    for (unsigned int i = 0; i < node_list.size(); i++) {
        edges = node_list[i]->get_edges_out();
        for (unsigned int e = 0; e < edges.size(); e++) {
            int start_id = edges[e]->start->id;
            int end_id   = edges[e]->end->id;
            if (!is_directed() and start_id > end_id) continue;
            if (!is_directed() and start_id == end_id) {
                Edge* comp = edges[e]->get_complement();
                if (edges[e]->id > comp->id) continue;
            }
            if (ot == NodeNames) {
                pipe << edges[e]->start->name << sep << edges[e]->end->name << endl;
            } else {
                pipe << start_id << sep << end_id << endl;
            }
        }
        if (node_list[i]->deg() == 0) {
            if (ot == NodeNames) {
                pipe << node_list[i]->name << endl;
            } else {
                pipe << node_list[i]->id << endl;
            }
        }
    }
    pipe.close();
}

/*
void Network::read_adj_matrix(string filename, char sep) {

    cerr << "Loading " << filename << endl;
    ifstream myfile(filename.c_str());
    std::stringstream ss;
    map<string,Node*> idmap;

    if (myfile.is_open()) {
        string line;

        int net_size = 0;
        int row_id = 0;

        while ( getline(myfile,line) ) {
            //split string based on sep and store results into vector
            vector<string> fields;
            split(line, sep, fields);
            const char whitespace[] = " \n\t\r";

            if (net_size == 0) {
                net_size = fields.size();
                this->populate(net_size);
            }

            if ((unsigned) net_size != fields.size()) {
                cerr << "Adjacenty matrix does not appear to be square.  Make sure matrix file has the same number of elements on each line.\n";
                exit(1);
            }

            for ( int col_id = 0; col_id<(signed) fields.size(); col_id++) {
                if (not is_directed() and row_id > col_id) continue;
                string val = strip(fields[col_id], whitespace);

                if (val == "1") {
                    get_node(row_id)->connect_to( get_node(col_id) );
                } else if (val == "0") {
                    continue;
                } else {
                    cerr << "Warning: unknown value found in adjacency matrix file (not 0 or 1): " << val << endl;
                }
            }
            row_id++;
        }
    }
    dumper();
    cerr << "finished dumping network\n";
    validate();
}*/

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

    for (int i = 0; i<size(); i++) {
        Node* node = node_list[i];
        int start = node->id;

        string start_name = to_string( node->get_name_or_id() );
        pipe << start << " [label=\"" << start_name << "\"]\n";
        if ( node->deg() == 0 ) {
            cerr << "Encountered unconnected node when trying to draw network.  This is currently not supported for graphviz output: node will be ignored.\n";
            continue;
        }

        vector<Edge*> edges_out = node->edges_out;
        for (unsigned int j = 0; j<edges_out.size(); j++) {
            Edge* edge = edges_out[j];
            int end = edge->end->id;

            string end_name = to_string( edge->end->get_name_or_id() );

            if ( seen_edges[end][start] > 0 ) {
                seen_edges[end][start]--;
                seen_edges[start][end]--;
                continue;
            } else {
                seen_edges[start][end]++;
            }
            string edge_op = directed ? " -> " : " -- ";
            int edge_id = edge->id;

            pipe << "\t\t" << start << edge_op << end << " [label=\"" << edge_id << "\"];\n";
        }
    }

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


Node::~Node() { /* cerr << "~Node called on node with id " << id << endl; */ }


void Node::delete_node() {
    // cerr << "Deleting node with id " << id << endl;
    while(edges_in.size() > 0)  { edges_in.back()->delete_edge(); }
    while(edges_out.size() > 0) { edges_out.back()->delete_edge(); }

    vector<Node*>::iterator itr = find(network->node_list.begin(), network->node_list.end(), this);
    network->node_list.erase(itr);

    network->set_topology_altered(true);
    delete this;
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


int Node::deg () const {
    return edges_out.size();
}


Edge* Node::get_rand_edge() {
    assert(deg() > 0);
    return edges_out[ rand_uniform_int(0, deg() - 1, network->get_rng()) ];
}


vector<Node*> Node::get_neighbors () const {
    vector<Node*> neighbors;
    for (unsigned int i = 0; i < edges_out.size(); i++) {
        neighbors.push_back(edges_out[i]->end);
    }
    return neighbors;
}


bool Node::is_neighbor (Node* node2) const {
    vector<Node*> neighbors = get_neighbors();
    for (unsigned int i = 0; i < neighbors.size(); i++) if (neighbors[i] == node2) return true;
    return false;
}


// Move an edge going from this node from a "current" neighbor to a "future" neighbor
bool Node::change_neighbors (Node* current, Node* future) {
    // try to find an edge going to "current" node
    Edge* edge_in = NULL;
    Edge* edge_out = NULL;

    for (unsigned int i = 0; i < edges_out.size(); i++) {
        if ( current == edges_out[i]->end ) {
            edge_out = edges_out[i];
            break;
        }
    }

    if (edge_out == NULL) {
        return false;
    } else {
        edge_out->break_end();
        edge_out->define_end(future);
    }

    if (! get_network()->is_directed()) {
        for (unsigned int i = 0; i < edges_in.size(); i++) {
            if ( current == edges_in[i]->start ) {
                edge_in = edges_in[i];
                break;
            }
        }
        if (edge_in == NULL) {
            return false;
        } else {
            edge_in->_move_edge(future);
        }
    }

    return true;
}


void Node::connect_to (Node* end) {
    Edge* edge1 = add_stub_out();
    edge1->define_end(end);
    if ( ! network->is_directed()) {
        Edge* edge2 = end->add_stub_out();
        edge2->define_end(this);
    }
}


bool Node::disconnect() {
    set<Edge*> edges_to_delete;
    for(Edge* e: get_edges_out()) {
        edges_to_delete.insert(e);
        if (not get_network()->is_directed()) {
            edges_to_delete.insert(e->get_complement());
        }
    }

    bool was_connected = (bool) edges_to_delete.size(); // false if 0, true otherwise
    while (edges_to_delete.size() > 0) {
        (*edges_to_delete.begin())->delete_edge();
        edges_to_delete.erase(edges_to_delete.begin());
    }
    return was_connected;
}


bool Node::disconnect_from (Node* end) {
    set<Edge*> edges_to_delete;
    for(Edge* e: get_edges_out()) {
        if (e->get_end() == end) {
            edges_to_delete.insert(e);
            if (not get_network()->is_directed()) {
                edges_to_delete.insert(e->get_complement());
            }
        }
    }
    bool were_connected = (bool) edges_to_delete.size(); // false if 0, true otherwise
    while (edges_to_delete.size() > 0) {
        (*edges_to_delete.begin())->delete_edge();
        edges_to_delete.erase(edges_to_delete.begin());
    }
    return were_connected;
}


void Node::_add_inbound_edge (Edge* edge) {
    edges_in.push_back(edge);
    network->set_topology_altered(true);
}


void Node::_add_outbound_edge (Edge* edge) {
    edges_out.push_back(edge);
    network->set_topology_altered(true);
}


//this doesn't delete the edge object, it merely disconnects it from the node that it was going to.
void Node::_del_inbound_edge (Edge* inbound) {
    if ( inbound->end->id != this->id ) {
        cerr << "The 'inbound' edge does not connect to the node provided." << endl;
        exit(100);
    }
    vector<Edge*>::iterator itr = find(edges_in.begin(), edges_in.end(), inbound);
    edges_in.erase(itr);
}


//this doesn't delete the edge object, it merely disconnects it from the node that it was coming from.
void Node::_del_outbound_edge (Edge* outbound) {
    if ( outbound->start->id != this->id ) {
        cerr << "The 'outbound' edge does not start from the node provided." << endl;
        exit(101);
    }
    vector<Edge*>::iterator itr = find(edges_out.begin(), edges_out.end(), outbound);
    edges_out.erase(itr);
}


bool Node::operator==( const Node& n2 ) {
    if ( id == n2.id) return true;
    return false;
}


ostream& operator<< (ostream &out, Node* node) {
    out << node->id;
    return out;
}


void Node::dumper() const {

    cerr << "\tname => " << name << endl;
    cerr << "\tid => "<< id << endl;
    cerr << "\tdegree => " << deg() <<  endl;
    cerr << "\tlocation => ";
    copy( loc.begin(), loc.end(), ostream_iterator<double>(cerr, " "));
    cerr << endl;

    vector<Edge*>::const_iterator itr;

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
                                 // Illogically, NAN != NAN is true
    if (mean != mean) cerr << "Mean_minimum_path is not meaningful for one-node components.\n" << endl;
    return mean;
}


double Node::min_path(Node* dest) {
    vector<Node*> empty(0);
    vector<double> distances = this->min_paths(empty);
    return distances[dest->id];
}


DistanceMatrix Node::_min_unweighted_paths(vector<Node*>& nodes) const {
    if (nodes.size() == 0) nodes = get_network()->node_list;
    DistanceMatrix known_cost;
    queue<const Node*> Q; // nodes to examine next
    vector<double> distances(nodes.size(), -1);

    map <const Node*, int> hits; // checking for existence is faster with a map
    for (unsigned int i = 0; i < nodes.size(); i++) {
       hits[ nodes[i] ] = 1;
    }

    known_cost[this] = 0;  //We only know initially that there is no cost to get to the starting node
    Q.push(this);

    int j = hits.count(this); //How many shortest paths we know for nodes in 'nodes' variable
    while ( ! Q.empty() ) {
        if (get_network()->process_stopped) {return known_cost;}
        const Node* known_node = Q.front();
        Q.pop();

        //Get the outbound edges for this known node
        vector <Node*> neighbors = known_node->get_neighbors();
        for (unsigned int i = 0; i < neighbors.size(); i++) {
            Node* v = neighbors[i];

            // if we've already done better, continue
            if (known_cost.count(v) == 1) {
                continue;
            } else {
                known_cost[v] = known_cost[known_node] + 1;
                Q.push(v);
                if (hits.count(v) == 1) {
                    j++;
                }
                if (j == (int) nodes.size()) { // we've found all the nodes we want
                    return known_cost;
                }
            }
        }
    }

    return known_cost;
}


bool MapNodeComp::operator() (const Node* const& lhs, const Node* const& rhs) const {return lhs->get_id() < rhs->get_id();}


DistanceMatrix Node::min_path_map(vector<Node*>& nodes) const {
    DistanceMatrix known_cost;
    if (network->is_weighted()) {
        known_cost = _min_paths(nodes);
    } else {
        known_cost = _min_unweighted_paths(nodes);
    }

    return known_cost;
}


vector<double> Node::min_paths(vector<Node*>& nodes) const {
    DistanceMatrix known_cost = this->min_path_map(nodes);

    vector<double> distances(nodes.size(), -1);
    for ( unsigned int i=0; i<nodes.size(); i++) {
        if (known_cost.count(nodes[i]) == 1) {
            distances[i] = known_cost[nodes[i]];
        }
    }

    return distances;
}


// Calculates length of the minimum path (if possible) between *this* and everything in *nodes*
// If *nodes* is empty, default is all nodes
DistanceMatrix Node::_min_paths(vector<Node*>& nodes) const {
    if (nodes.size() == 0) nodes = get_network()->node_list;
    DistanceMatrix known_cost; //Per Dijkstra's Algorithm, these are the two lists
    DistanceMatrix::iterator itr;
                                 //we need to keep track of
    DistanceMatrix uncertain_cost;

                                 //the following initializes the 'uncertain' set with undefined values,
                                 //since we have no information about these nodes yet.
    for (unsigned int i = 0; i < nodes.size(); i++) {
        if (this == nodes[i]) continue;
                                 // infinity =: max double value
        uncertain_cost[ nodes[i] ] = std::numeric_limits<double>::max();
    }
    known_cost[this] = 0;        //We only know initially that there is no cost to get to the starting node

    int j = 0;
                                 //As long as there are nodes with uncertain min costs
    while ( j++ < (signed) nodes.size() ) {
        if (get_network()->process_stopped) {return known_cost;}

        Node* min = NULL;
                                 //Loop through the nodes we know about.
        for ( itr = known_cost.begin(); itr != known_cost.end(); itr++) {
                                 //key of the map is the node
            const Node* known_node = (*itr).first;
                                 //Get the outbound edges for this known node

            vector <Edge*> edges = known_node->edges_out;
            for (unsigned int i = 0; i < edges.size(); i++) {
                                 //Get this neighbor
                Node* neighbor = edges[i]->end;
                                 //Move on if this endpoint already has a known cost
                if ( known_cost.count(neighbor) > 0 ) continue;
                                 //Otherwise, calculate a cost using this path
                double cost = known_cost[known_node] + edges[i]->cost;

                //Store the new cost as an uncertain cost
                //if it's better than the others we've seen
                if (uncertain_cost.count(neighbor) > 0 && uncertain_cost[neighbor] < cost) {
                    continue;
                } else {
                    if ( min == NULL ) min = neighbor;
                    uncertain_cost[neighbor] = cost;
                    if ( uncertain_cost[neighbor] < uncertain_cost[min] ) min = neighbor;
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

    return known_cost;
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


Edge::~Edge() {  /* cerr << "~Edge called on edge with id " << id << endl; */ }


void Edge::delete_edge() {
    vector<Edge*>::iterator itr;

    if (end != NULL) {
        itr = find(end->edges_in.begin(), end->edges_in.end(), this);
        if (itr != end->edges_in.end()) {
            end->edges_in.erase(itr);
        }
    }

    if (start != NULL) {
        itr = find(start->edges_out.begin(), start->edges_out.end(), this);
        if (itr != start->edges_out.end()) {
            start->edges_out.erase(itr);
        }
    }

    /* Using a reverse iterator seems to be slightly faster,
       but it's substantially more obscure

    vector<Edge*>::reverse_iterator rit;

    if (end != NULL) {
        rit = find(end->edges_in.rbegin(), end->edges_in.rend(), this);
        if (rit != end->edges_in.rend()) {
            end->edges_in.erase(--(rit.base()));
        }
    }

    if (start != NULL) {
        rit = find(start->edges_out.rbegin(), start->edges_out.rend(), this);
        if (rit != start->edges_out.rend()) {
            start->edges_out.erase(--(rit.base()));
        }
    }
    */

    network->set_topology_altered(true);
    delete this;
}


void Edge::disconnect_nodes() {
    Edge* comp = get_complement();

    delete_edge();
    comp->delete_edge();
}


void Edge::set_cost(double c) {
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


void Edge::break_end () {
    if (end == NULL) return;
    end->_del_inbound_edge(this);
    end = NULL;
    network->set_topology_altered(true);
}


void Edge::define_end (Node* end_node) {
    end = end_node;
    end->_add_inbound_edge(this);
    network->set_topology_altered(true);
}


void Edge::_move_edge (Node* new_start_node) {
    if (start != NULL) {
        start->_del_outbound_edge(this);
        start = NULL;
    }
    start = new_start_node;
    start->_add_outbound_edge(this);
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


void Edge::dumper () const {
    string space = "\t\t";
    string start_name = start->get_name_or_id();
    string end_name = ( end != NULL ) ? end->get_name_or_id() : "undef";
    cerr << space << "id => " << id << ", start => " << start_name << ", end => " << end_name << endl;
}
