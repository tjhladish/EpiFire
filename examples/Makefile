CFLAGS=--ansi --pedantic -O2
#CFLAGS=--ansi --pedantic -g 
INCLUDE= -I../src/
LDFLAGS=  ../src/*.o

all: epifire gsl test_network path_length_test ex1_mass_action ex2_percolation ex3_chain_binomial ex4_dynamic_net ex5_diff_eq ex6_network_diff_eq ex7_gillespie_network_SEIRS mod_ex7_gillespie_network_SEIRS 

epifire: 
	$(MAKE) -C ../src/

gsl:
	$(MAKE) -C ../gsl_subset/

test_network: test_network.cpp 
	g++ $(CFLAGS) test_network.cpp $(INCLUDE) $(LDFLAGS) -o test_network  

path_length_test: path_length_test.cpp 
	g++ $(CFLAGS) path_length_test.cpp $(INCLUDE) $(LDFLAGS) -o path_length_test  

ex1_mass_action: ex1_mass_action.cpp
	g++ $(CFLAGS) ex1_mass_action.cpp $(INCLUDE) $(LDFLAGS) -o ex1_mass_action

ex2_percolation: ex2_percolation.cpp
	g++ $(CFLAGS) ex2_percolation.cpp $(INCLUDE) $(LDFLAGS) -o ex2_percolation

ex3_chain_binomial: ex3_chain_binomial.cpp
	g++ $(CFLAGS) ex3_chain_binomial.cpp $(INCLUDE) $(LDFLAGS) -o ex3_chain_binomial

ex4_dynamic_net: ex4_dynamic_net.cpp
	g++ $(CFLAGS) ex4_dynamic_net.cpp $(INCLUDE) $(LDFLAGS) -o ex4_dynamic_net

ex5_diff_eq: ex5_diff_eq.cpp
	g++ $(CFLAGS) ex5_diff_eq.cpp $(INCLUDE) -I../gsl_subset/ $(LDFLAGS) ../gsl_subset/*.o -o ex5_diff_eq

ex6_network_diff_eq: ex6_network_diff_eq.cpp
	g++ $(CFLAGS) ex6_network_diff_eq.cpp $(INCLUDE) -I../gsl_subset/ $(LDFLAGS) ../gsl_subset/*.o -o ex6_network_diff_eq

ex7_gillespie_network_SEIRS: ex7_gillespie_network_SEIRS.cpp
	g++ $(CFLAGS) ex7_gillespie_network_SEIRS.cpp $(INCLUDE) $(LDFLAGS) -o ex7_gillespie_network_SEIRS

mod_ex7_gillespie_network_SEIRS: mod_ex7_gillespie_network_SEIRS.cpp
	g++ $(CFLAGS) mod_ex7_gillespie_network_SEIRS.cpp $(INCLUDE) $(LDFLAGS) -o mod_ex7_gillespie_network_SEIRS
	
dynamic_network_edge_distros: dynamic_network_edge_distros.cpp
	g++ $(CFLAGS) dynamic_network_edge_distros.cpp $(INCLUDE) $(LDFLAGS) -o dynamic_network_edge_distros

clean:
	rm -f test_network ex1_mass_action ex2_percolation ex3_chain_binomial ex4_dynamic_net ex5_diff_eq ex6_network_diff_eq ex7_gillespie_network_SEIRS mod_ex7_gillespie_network_SEIRS
