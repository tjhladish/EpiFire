CFLAGS=--ansi --pedantic -O2
#CFLAGS=--ansi --pedantic -g 
INCLUDE= -I../src/
LDFLAGS=  ../src/*.o

all: epifire test_network ex1_mass_action ex2_percolation ex3_chain_binomial test_rng

epifire: 
	$(MAKE) -C ../src/

path_length_test: path_length_test.cpp 
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) path_length_test.cpp -o path_length_test  

test_network: test_network.cpp 
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) test_network.cpp -o test_network  

ex1_mass_action: ex1_mass_action.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex1_mass_action.cpp -o ex1_mass_action

ex2_percolation: ex2_percolation.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex2_percolation.cpp -o ex2_percolation

ex3_chain_binomial: ex3_chain_binomial.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex3_chain_binomial.cpp -o ex3_chain_binomial

ex4_dynamic_net: ex4_dynamic_net.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) ex4_dynamic_net.cpp -o ex4_dynamic_net

ex5_diff_eq: ex5_diff_eq.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) -L/usr/local/lib -lgsl -lblas ex5_diff_eq.cpp -o ex5_diff_eq

ex6_network_diff_eq: ex6_network_diff_eq.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) -L/usr/local/lib -lgsl -lblas ex6_network_diff_eq.cpp -o ex6_network_diff_eq

test_rng: test_rng.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) test_rng.cpp -o test_rng

ex5b_diff_eq: ex5_diff_eq.cpp
	g++ $(CFLAGS) -o ex5b_diff_eq ex5_diff_eq.cpp $(INCLUDE) $(LDFLAGS) -L../gsl_subset -lgsl_subset -I../gsl_subset 

clean:
	rm -f test_network ex1_mass_action ex2_percolation ex3_chain_binomial
