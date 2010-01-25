#include "Utility.h"

double EPSILON = 10e-15; // probabilities smaller than this are treated as zero

/*!
 * @function generate Truncated poisson distribution
 * @abstract bla bla
 * @discussion Use HMBalloonRect to get information about the size of a help balloon
 * before the Help Manager displays it.
 * @param lambda   
 * @param min sd
 * @param max
*/
        // generate a truncated poisson distribution
vector<double> gen_trunc_poisson(double lambda, int min, int max) {
    vector<double> dist(max + 1);       //initializes distribution to all 0
    double sum = 0;

    for (int k = min; k <= max; k++) {
        //cerr << "\tk: " << k;
        double a = exp(-lambda);
        double b = pow(lambda,k);
        double c = factorial(k);
        double pk = a * b / c; 
        dist[k] = pk;
        sum += pk;
        //cerr << "\tsum: " << sum << endl;
        if (pk/sum < EPSILON) {
            dist.resize(k + 1);
            break;
        }
    }

    return normalize_dist(dist, sum);
}

        // generate a truncated, discretized powerlaw distribution (= zeta dist?)
vector<double> gen_trunc_powerlaw(double alpha, double kappa, int min, int max) {
    vector<double> dist(max + 1);
    double sum = 0;

    for (int k = min; k <= max; k++) {
        double pk = pow(k, -alpha) * exp(-k/kappa);
        dist[k] = pk;
        sum += pk;
        if (pk/sum < EPSILON) break;
    }
                                // Norm the distribution
    return normalize_dist(dist, sum);
}


vector<double> gen_trunc_exponential(double lambda, int min, int max) {
    vector<double> dist(max + 1);
    double sum = 0;

    for (int k = min; k <= max; k++) {
        double pk = lambda * exp(-lambda * k);
        dist[k] = pk;
        sum += pk;
        if (pk/sum < EPSILON) break;
    }
                                // Norm the distribution
    return normalize_dist(dist, sum);
}


vector<double> normalize_dist(vector<double> dist, double sum) {
    for ( int i = 1; i < dist.size(); i++) dist[i] = dist[i] / sum;
    return dist;
}

int rand_nonuniform_int(vector<double> dist, MTRand* mtrand) {
    //assert(sum(dist) == 1);  // why doesn't this work?
    double last = 0;
    double rand = mtrand->rand();
    for ( int i = 0; i < dist.size(); i++ ) {
       
        double current = last + dist[i];
        if ( rand <= current ) {
            return i;
        } else {
            last = current;
        }
    }
    if (last != 1) {
        cerr << "rand_uniform_integer() expects a normed distribution.  "
             << "Your probabilities sum to " << setprecision(15) << last
             << ".  Fix this using Utilities::normalize_dist()\n";
        exit(1);
    }
}

double rand_exp(double lambda, MTRand* mtrand) {
    return -log(mtrand->rand()) / lambda;
}

// N is the size of the sample space--which includes 0, so the int "N" itself will never get
// returned in the sample.  sample is an empty vector that needs to be of size k; mtrand
// is a Mersenne Twister RNG.
void rand_nchoosek(int N, vector<int>& sample, MTRand* mtrand) {
            
    if (sample.size() == 0 ) return;
    int k = sample.size();  // k is specified by size of requested vector

    assert( k <= N ); 

    int top = N-k;          
    double Nreal = (double) N;

    int newidx=0;
    int lastidx=0;
    int i=0;

    while ( k >= 2 ){
         double V = mtrand->rand();
         int S=0;
         double quot = top/Nreal;
         while( quot > V ) {
            S++; top-=1; Nreal-=1;
            quot =(quot*top)/Nreal;
        }   
        //skip over the next S records and select the following one for the sample
        newidx = lastidx + S;
        sample[i]=newidx; lastidx = newidx+1; i++;
        Nreal -= 1.0; k -= 1;
    }
    
    if ( k == 1 ) {
        // the following line had (newidx+1) instead of lastidx before, which
        // produced incorrect results when N == 1; this, I believe, is correct
        sample[i++] = lastidx + (int) mtrand->rand( (int) Nreal );
    }
}
/*
double sum(vector<double> list) {
    double sum = 0;
    for (int i = 0; i < list.size(); i++) {
        sum += list[i];
    }
    return sum;
}

int sum(vector<int> list) {
    int sum = 0;
    for (int i = 0; i < list.size(); i++) {
        sum += list[i];
    }
    return sum;
}


template <typename T>
T sum(vector<T> list) {
    T sum = 0;
    for (int i = 0; i < list.size(); i++) {
        sum += list[i];
    }
    return sum;
}

template <typename T>
double mean(vector<T> list) {
    return (double) sum(list) / list.size();
}*/

double factorial(int num) {
    assert( num > -1);
    if (num == 0) return 1; // definition of 0!
    double result = log(1);
    for (int i = 1; i <= num; i++) result = result + log(i);
    return exp(result);
}

int max_element(vector<int> list) {
    int element = list[0];
    for (int i = 0; i < list.size(); i++) {
        element = max(element, list[i]);
    }
    return element;
}


void split(const string& s, char c, vector<string>& v) {
    string::size_type i = 0;
    string::size_type j = s.find(c);

    while (j != string::npos) {
       v.push_back(s.substr(i, j-i));
       i = ++j;
       j = s.find(c, j);

       if (j == string::npos) v.push_back(s.substr(i, s.length( )));
    }
}


/*      	// initialize random seed:
void  seed_rand() { 
    if (called_seed) return;
  	srand ( time(NULL) );
    long seed1 = rand();
    long seed2 = rand();
    setall(seed1,seed2);
    called_seed = true;
}*/


/*
public static void shuffle (int[] array)
    {
        Random rng = new Random();   // i.e., java.util.Random.
        int n = array.length;        // The number of items left to shuffle (loop invariant).
        while (n > 1)
        {
            int k = rng.nextInt(n);  // 0 <= k < n.
            --n;                     // n is now the last pertinent index;
            int temp = array[n];     // swap array[n] with array[k].
            array[n] = array[k];
            array[k] = temp;
        }
    }
*/
