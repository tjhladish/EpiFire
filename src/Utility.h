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

double factorial (int num);
int max_element(vector<int> list);

vector<double> gen_trunc_poisson (double lambda, int min, int max);
vector<double> gen_trunc_exponential (double lambda, int min, int max);
vector<double> gen_trunc_powerlaw (double alpha, double kappa, int min, int max);
vector<double> normalize_dist (vector<double> dist, double sum);
vector<double> normalize_dist (vector<int> dist, int sum);
int rand_nonuniform_int (vector<double> dist, MTRand* mtrand);
double rand_exp (double lambda, MTRand* mtrand);

void rand_nchoosek(int n, vector<int>& sample, MTRand* mtrand);

void split(const string& s, char c, vector<string>& v);
string strip (string const& str, char const* sepSet);

template <typename T>
inline std::string to_string (const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
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
inline T min(const T& _a, const T& _b) { return ((_a)<(_b)?(_a):(_b));}

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


inline vector<int> tabulate_vector( vector<int> my_vector ) {
    vector<int> tabulated(max_element(my_vector));
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
