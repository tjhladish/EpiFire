#ifndef UTILITY_H
#define UTILITY_H

#include <cstdlib>
#include <sstream>
#include <vector>
#include <assert.h>
#include <iterator>
#include <iomanip>
#include "MersenneTwister.h"

using namespace std;

template <typename T> inline T sum(vector<T> list) { T sum=0; for (unsigned int i=0; i<list.size(); i++) sum += list[i]; return sum;}
template <typename T> inline double mean(vector<T> list) { return (double) sum(list) / list.size(); }
template <typename T> 
inline double median(vector<T> L) { 
    sort(L.begin(), L.end());
    int len = L.size();
    if (len%2 == 1) { // odd number of elements
        return L[(len-1)/2];
    } else { 
        return (L[len/2] + L[(len-2)/2])/2.0;
    }
}

long double factorial (int num);
int min_element(vector<int> list);
int max_element(vector<int> list);

long double poisson_pmf(double lambda, int k);
vector<double> gen_trunc_poisson (double lambda, int min, int max);
vector<double> gen_trunc_exponential (double lambda, int min, int max);
vector<double> gen_trunc_powerlaw (double alpha, double kappa, int min, int max);
int rand_nonuniform_int (vector<double> dist, MTRand* mtrand);
double rand_exp (double lambda, MTRand* mtrand);

void rand_nchoosek(int n, vector<int>& sample, MTRand* mtrand);
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
inline std::string to_string (const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

inline float to_float(const std::string& s){
    std::istringstream i(s);
    float x = 0;
    i >> x;
    return x;
}

template <typename T>
double stdev(vector<T> & numbers) {
    double x = mean(numbers);
    double var_num = 0;
    int N = numbers.size();
    for (int i=0; i<N; i++) var_num += pow(numbers[i] - x, 2);
    double sd = sqrt(var_num/(N-1));
    return sd;
}


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
inline void cerr_vector(vector<T> & my_vector) {
    for (int i = 0; i < my_vector.size(); i++ ) cerr << my_vector[i] << " ";
    cerr << endl;
}


template <typename T>
inline void shuffle(vector<T> & my_vector, MTRand* mtrand) {
    int max = my_vector.size() - 1;
    for (int i = max; i >= 0; i-- ) swap(my_vector[i], my_vector[ mtrand->randInt(i) ]);
}
#endif
