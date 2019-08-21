#ifndef UTILITY_H
#define UTILITY_H

#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <iterator>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <random>

using namespace std;

template <typename T> inline T sum(vector<T> list) { T sum=0; for (unsigned int i=0; i<list.size(); i++) sum += list[i]; return sum;}
template <typename T> inline double mean(vector<T> list) { return (double) sum(list) / list.size(); }

template <typename T> inline
double median(vector<T> L) { 
    sort(L.begin(), L.end());
    float idx = (L.size() - 1.0) * 0.5;
    return ( L[ (int) ceil(idx) ] + L[ (int) floor(idx) ] ) /2.0;
}

// five number summary (min, 1st quartile, median, 3rd quartile, max)
template <typename T> inline 
vector<double> fivenum(vector<T> L) {
    assert(L.size() > 2);
    vector<double> stats(5);
    sort(L.begin(), L.end());
    stats[0] = L[0];        // min
    stats[4] = L.back();    // max

    float idx1 = (L.size() -1) * 0.25;
    float idx2 = (L.size() -1) * 0.5;
    float idx3 = (L.size() -1) * 0.75;
    
    stats[1] = (L[ceil(idx1)] + L[floor(idx1)]) /2.0;
    stats[2] = (L[ceil(idx2)] + L[floor(idx2)]) /2.0;
    stats[3] = (L[ceil(idx3)] + L[floor(idx3)]) /2.0;
 
    return stats;
}


long double factorial (int num);

template <typename T> inline 
T min_element(vector<T> list) {
    T element = list[0];
    for (unsigned int i = 0; i < list.size(); i++) {
        element = min(element, list[i]);
    }
    return element;
}


template <typename T> inline 
T max_element(vector<T> list) {
    T element = list[0];
    for (unsigned int i = 0; i < list.size(); i++) {
        element = max(element, list[i]);
    }
    return element;
}

template <typename T> inline 
T range(vector<T> list) {
    return max_element(list) - min_element(list);
}



long double poisson_pmf(double lambda, int k);
vector<double> gen_trunc_poisson (double lambda, int min, int max);
vector<double> gen_trunc_exponential (double lambda, int min, int max);
vector<double> gen_trunc_powerlaw (double alpha, double kappa, int min, int max);

int rand_nonuniform_int (vector<double> dist, mt19937* rng);
int rand_uniform_int (int min, int max, mt19937* rng);
double rand_uniform (double min, double max, mt19937* rng);
double rand_normal (double mean, double std_dev, mt19937* rng);
double rand_exp (double lambda, mt19937* rng);
int rand_binomial (int n, double p, mt19937* rng);

void rand_nchoosek(int n, vector<int>& sample, mt19937* rng);
double normal_pdf(double x, double mu, double var);
double normal_cdf(double x, double mu, double var);
 
void split(const string& s, char c, vector<string>& v);
string strip (string const& str, char const* sepSet);

template <typename T>
vector<double> normalize_dist (vector<T> dist, T sum) {
    vector<double> normed(dist.size());
    for (unsigned int i = 0; i < dist.size(); i++) normed[i] = ((double) dist[i]) / sum;
    return normed;
}

template <typename T>
vector<double> normalize_dist (vector<T> dist) {
    return normalize_dist(dist, sum(dist));
}

template <typename T>
inline string to_string (const T& t) {
    stringstream ss;
    ss << t;
    return ss.str();
}

inline float to_float(const string& s){
    istringstream i(s);
    float x = 0;
    i >> x;
    return x;
}

inline double to_double(const string& s){
    istringstream i(s);
    double x = 0;
    i >> x;
    return x;
}

inline int to_int(const string& s){
    istringstream i(s);
    int x = 0;
    i >> x;
    return x;
}

template <typename T>
double variance(vector<T> & numbers) {
    double x = mean(numbers);
    double var_num = 0;
    int N = numbers.size();
    for (int i=0; i<N; i++) var_num += pow(numbers[i] - x, 2);
    double var = var_num/(N-1);
    return var;
}

template <typename T>
double stdev(vector<T> & numbers) { return sqrt( variance(numbers) ); }

template <typename T>
inline int sign(const T& _a) { return (int)((_a)<0 ? (-1) : (1)); }

template <typename T>
inline T MIN(const T& _a, const T& _b) { return ((_a)<(_b)?(_a):(_b));}

template <typename T>
inline T MAX(const T& _a,const T& _b) { return ((_a)>(_b)?(_a):(_b));}

template <typename T>
inline void delete_element(vector<T> & my_vector, T element) {
    for(int i = 0; i < my_vector.size(); i++) {
        if (my_vector[i] == element) {
            my_vector.erase(my_vector.begin() + i);
            break;
        }
    }
}


inline vector<int> tabulate_vector( vector<int> & my_vector ) {
    vector<int> tabulated(max_element(my_vector) + 1, 0);
    for (unsigned int i = 0; i<my_vector.size(); i++) tabulated[my_vector[i]]++;
    return tabulated;
}


template <typename T>
inline void cerr_vector(vector<T> & my_vector, string sep = " ") {
    for (int i = 0; i < my_vector.size() - 1; i++ ) cerr << my_vector[i] << sep;
    cerr << my_vector.back();
}


template <typename T>
inline void cout_vector(vector<T> & my_vector, string sep = " ") {
    for (int i = 0; i < my_vector.size() - 1; i++ ) cout << my_vector[i] << sep;
    cout << my_vector.back();
}


inline double string2double(const string& s){ istringstream i(s); double x = 0; i >> x; return x; }

vector<double> read_vector_file(string filename);

vector<vector<double> > read_2D_vector_file(string filename, char sep);

template <typename T> //TODO: could use new shuffle algorithm
inline void shuffle(vector<T> & my_vector, mt19937* rng) {
    int max = my_vector.size() - 1;
    for (int i = max; i >= 0; i-- ) {
        swap(my_vector[i], my_vector[ rand_uniform_int(0, i, rng) ]);
    }
}
#endif
