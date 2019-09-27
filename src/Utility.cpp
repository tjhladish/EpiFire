#include "Utility.h"

double EPSILON = 10e-15;         // probabilities smaller than this are treated as zero

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
long double poisson_pmf(double lambda, int k) {
    long double a = expl(-lambda);
    long double b = powl((long double) lambda,k);
    long double c = factorial(k);
    return a * b / c;
}

vector<double> gen_trunc_poisson(double lambda, int min, int max) {
    vector<double> dist(max + 1, 0.0);//initializes distribution to all 0
    double sum = 0.0;
    if (lambda < 500) {
        for (int k = (int) lambda; k >= min; k--) {
            dist[k] = poisson_pmf(lambda, k);
            sum += dist[k];
            if ( dist[k]/sum < EPSILON) break;
        }
        
        for (int k = (int) lambda + 1; k <= max; k++) {
            dist[k] = poisson_pmf(lambda, k);
            sum += dist[k];
            if ( dist[k]/sum < EPSILON) {
                dist.resize(k + 1);
                break;
            }
        }

    } else { // use a normal approximation, avoiding the factorial and pow calculations
    // by starting 9 SD's above lambda, we should capture all densities greater than EPSILON
        int prob_max = (int) (lambda + 9.0 * sqrt(lambda));
        max = MIN(max, prob_max);
        dist.resize(max + 1);
        
        for (int k = max; k >= min; k--) {
            dist[k] = normal_cdf(k + 0.5, lambda, lambda); // 0.5 is a continuity correction
            if ( k < max ) {
                dist[k+1] -= dist[k];
                sum += dist[k+1];
            }
            if ( k < lambda and dist[k+1] < EPSILON) {
                dist[k] = 0;
                break;
            }
        }
            
    }
    return normalize_dist(dist, sum);
}


// generate a truncated, discretized powerlaw distribution (= zeta dist?)
vector<double> gen_trunc_powerlaw(double alpha, double kappa, int min, int max) {
    vector<double> dist(max + 1);
    double sum = 0;

    for (int k = min; k <= max; k++) {
        double pk = powl(k, (long double) -alpha) * expl(-k/(long double) kappa);
        dist[k] = pk;
        sum += pk;
        if (pk/sum < EPSILON) break;
    }
    // Norm the distribution
    return normalize_dist(dist, sum);
}


vector<double> gen_trunc_exponential(double lambda, int min, int max) {
    vector<double> dist(max + 1);
    if (lambda <= 0) {
        cerr << "Exponential distribution must have a positive parameter value\n";
        return dist;
    }
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


int rand_nonuniform_int(vector<double> dist, std::mt19937* rng) {
    double last = 0;
    double rand = rand_uniform(0, 1, rng);
    for (unsigned int i = 0; i < dist.size(); i++ ) {

        double current = last + dist[i];
        if ( rand <= current ) {
            return i;
        } else {
            last = current;
        }
    }
    if (fabs(last - 1) > EPSILON) {
        cerr << "rand_nonuniform_int() expects a normed distribution.  "
             << "Your probabilities sum to " << setprecision(15) << last
             << ".  Fix this using Utilities::normalize_dist()\n";
        exit(1);
    }

    return dist.size() - 1;
}


int rand_uniform_int (int min, int max, std::mt19937* rng) {
    // uniform integer on [min, max] (inclusive)
    std::uniform_int_distribution<> dist(min, max);
    return dist(*rng);
}

   
double rand_uniform (double min, double max, std::mt19937* rng) {
    std::uniform_real_distribution<> dist(min, max);
    return dist(*rng);
}

double rand_normal(double mean, double std_dev, std::mt19937* rng) {
    std::normal_distribution<> dist(mean, std_dev);
    return dist(*rng);
}


double rand_exp(double lambda, std::mt19937* rng) {
    std::uniform_real_distribution<> dist(0, 1);
    return -log(dist(*rng)) / lambda; //TODO: could return inf if 0 happens to be returned
}


// Devroye's algorithm, as described by Kachitvichyanukul and Schmeiser (1988), 
// "Binomial random variate generation."  I've correct two mistakes in the publication:
// the original fails if p = 1, and the conditional for the while loop should be y <= n, 
// rather than y < n.  The latter precludes ever drawing a deviate of n for p < 1.

// Algorithm BG
// 1. Set y <-- 0, x <-- 0, c <-- ln(1 - p).
// 2. If c == 0, return x.
// 3. Generate u ~ U(0, 1).
// 4. y <-- y + floor(ln(u)/c) + 1.
// 5. If y < n, set x <-- x + 1, and goto 3.
// 6. Return x.

int rand_binomial (int n, double p, std::mt19937* rng) {
    if ( p == 1.0 ) {
        return n;
    } else if ( p == 0.0 or n == 0 ) {
        return 0;
    }

    int y = 0;
    int x = 0;
    double c = log( 1 - p ); // p can't be 0, but we've already checked that

    while ( y <= n ) {
        double u = rand_uniform(0, 1, rng);
        //assert(u > 0.0);
        y += (int) (log(u)/c) + 1;
        if (y > n) {
            return x;
        }
        x += 1;
    }
    return -1; // bad input was provided
}


// N is the size of the sample space--which includes 0, so the int "N" itself will never get
// returned in the sample.  sample is an empty vector that needs to be of size k; rng
// is a Mersenne Twister RNG.
void rand_nchoosek(int N, vector<int>& sample, std::mt19937* rng) {

    if (sample.size() == 0 ) return;
    int k = sample.size();       // k is specified by size of requested vector

    assert( k <= N );

    int top = N-k;
    double Nreal = (double) N;

    int newidx=0;
    int lastidx=0;
    int i=0;

    while ( k >= 2 ) {
        double V = rand_uniform(0, 1, rng);
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
        sample[i++] = lastidx + (int) rand_uniform(0, (int) Nreal, rng);
    }
}

double normal_pdf(double x, double mu, double var) {
    long double PI = 3.1415926535897932384;
    return exp(-pow(x-mu,2) / (2.0*var)) / sqrt(2*PI*var);
}

double normal_cdf(double x, double mu, double var) {
    x = (x-mu)/sqrt(var);
    // Abramowitz & Stegun (1964) approximation
    long double b0 = 0.2316419;
    double b1 = 0.319381530;
    double b2 = -0.356563782;
    double b3 = 1.781477937;
    double b4 = -1.821255978;
    double b5 = 1.330274429;
    if (x >= 0.0) {
        long double t = 1.0/(1.0+b0*x);
        return 1.0 - normal_pdf(x, 0, 1)*(b1*t + b2*pow(t,2) + b3*pow(t,3) + b4*pow(t,4) + b5*pow(t,5));
    } else {
        long double t = 1.0/(1.0-b0*x);
        return normal_pdf(x, 0, 1)*(b1*t + b2*pow(t,2) + b3*pow(t,3) + b4*pow(t,4) + b5*pow(t,5));
    }
}


long double factorial(int num) {
    assert( num > -1);
    if (num == 0) return 1;      // definition of 0!
    long double result = 0.0;//log(1);
    for (int i = 1; i <= num; i++) result = result + logl((long double) i);
    return expl(result);
}


std::string strip ( std::string const& str, char const* sepSet) {
    std::string::size_type const first = str.find_first_not_of(sepSet);
    return ( first==std::string::npos )
        ? std::string()
        : str.substr(first, str.find_last_not_of(sepSet)-first+1);
}


void split(const string& s, char c, vector<string>& v) {
    string::size_type i = 0;
    string::size_type j = s.find(c);

    while (j != string::npos) {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);
    }
    if (j == string::npos) v.push_back(s.substr(i, s.length( )));
}


vector<double> read_vector_file(string filename) {
//    cerr << "Loading " << filename << endl;
    ifstream myfile(filename.c_str());

    vector<double> M;
    if (myfile.is_open()) {
        string line;
        while ( getline(myfile,line) ) {
            M.push_back(string2double(line));
        }
    }
    return M;
}

vector<vector<double> > read_2D_vector_file(string filename, char sep) {
 //   cerr << "Loading " << filename << endl;
    ifstream myfile(filename.c_str());

    vector<vector<double> > M;
    if (myfile.is_open()) {
        string line;

        while ( getline(myfile,line) ) {
            vector<string> fields;
            split(line, sep, fields);

            vector<double>row(fields.size());
            for( unsigned int i=0; i < fields.size(); i++ ) {
                    row[i] = string2double(fields[i]);
            }
            M.push_back(row);
        }
    }
    return M;
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
