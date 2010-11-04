CFLAGS=--ansi --pedantic -O2
#CFLAGS=--ansi --pedantic -g 
INCLUDE= -I../src/
#LDFLAGS= -L../src/ -lsim 
LDFLAGS=  ../src/*.o

all: test_network ex1_mass_action ex2_percolation ex3_chain_binomial test_rng

test_network: test_network.cpp 
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) test_network.cpp -o test_network  

ex1_mass_action: ex1_mass_action.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex1_mass_action.cpp -o ex1_mass_action

ex2_percolation: ex2_percolation.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex2_percolation.cpp -o ex2_percolation

ex3_chain_binomial: ex3_chain_binomial.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex3_chain_binomial.cpp -o ex3_chain_binomial

test_rng: test_rng.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) test_rng.cpp -o test_rng


clean:
	rm -f test_network ex1_mass_action ex2_percolation ex3_chain_binomial

