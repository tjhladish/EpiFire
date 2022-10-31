CFLAGS=--ansi --pedantic -O2 -std=c++11
#CFLAGS=--ansi --pedantic -g 
INCLUDE= -I../src/
LDFLAGS=  ../src/*.o

ABCDIR = $(HOME)/work/AbcSmc
ABC_INC = -I$(ABCDIR) -I$(ABCDIR)/sqdb -I$(ABCDIR)/jsoncpp/include
GSL_INC = -I$(ABCDIR)/gsl_local/include
ABC_LIB = -L$(ABCDIR) -labc -ljsoncpp -lsqdb $(ABCDIR)/sqlite3.o
GSL_LIB = -lm -L$(ABCDIR)/gsl_local/lib/ -lgsl -lgslcblas -lpthread -ldl

all: epifire gsl test_network path_length_test ex1_mass_action ex2_percolation ex3_chain_binomial ex4_dynamic_net ex5_diff_eq ex6_network_diff_eq ex7_gillespie_network_SEIRS 

epifire: 
	$(MAKE) -C ../src/

gsl:
	$(MAKE) -C ../gsl_subset/

test_network: test_network.cpp epifire
	g++ $(CFLAGS) test_network.cpp $(INCLUDE) $(LDFLAGS) -o test_network  

path_length_test: path_length_test.cpp epifire
	g++ $(CFLAGS) path_length_test.cpp $(INCLUDE) $(LDFLAGS) -o path_length_test  

ex1_mass_action: ex1_mass_action.cpp
	g++ $(CFLAGS) ex1_mass_action.cpp $(INCLUDE) $(LDFLAGS) -o ex1_mass_action

libabc:
	$(MAKE) -C $(ABCDIR) -f Makefile

sacema60_sim: libabc main_sql60k.cpp
	g++ $(CFLAGS) main_sql60k.cpp $(INCLUDE) $(ABC_INC) $(GSL_INC) $(LDFLAGS) -o sacema_sim_60k $(ABC_LIB) $(GSL_LIB)

sacema75_sim: libabc main_sql75k.cpp
	g++ $(CFLAGS) main_sql75k.cpp $(INCLUDE) $(ABC_INC) $(GSL_INC) $(LDFLAGS) -o sacema_sim_75k $(ABC_LIB) $(GSL_LIB)

ex2_percolation: ex2_percolation.cpp epifire
	g++ $(CFLAGS) ex2_percolation.cpp $(INCLUDE) $(LDFLAGS) -o ex2_percolation

ex3_chain_binomial: ex3_chain_binomial.cpp epifire
	g++ $(CFLAGS) ex3_chain_binomial.cpp $(INCLUDE) $(LDFLAGS) -o ex3_chain_binomial

ex4_dynamic_net: ex4_dynamic_net.cpp
	g++ $(CFLAGS) ex4_dynamic_net.cpp $(INCLUDE) $(LDFLAGS) -o ex4_dynamic_net

ex5_diff_eq: ex5_diff_eq.cpp gsl
	g++ $(CFLAGS) ex5_diff_eq.cpp $(INCLUDE) -I../gsl_subset/ $(LDFLAGS) ../gsl_subset/*.o -o ex5_diff_eq

ex6_network_diff_eq: ex6_network_diff_eq.cpp gsl
	g++ $(CFLAGS) ex6_network_diff_eq.cpp $(INCLUDE) -I../gsl_subset/ $(LDFLAGS) ../gsl_subset/*.o -o ex6_network_diff_eq

ex7_gillespie_network_SEIRS: ex7_gillespie_network_SEIRS.cpp
	g++ $(CFLAGS) ex7_gillespie_network_SEIRS.cpp $(INCLUDE) $(LDFLAGS) -o ex7_gillespie_network_SEIRS

ex8_event_driven_SEIRS: ex8_event_driven_SEIRS.cpp
	g++ $(CFLAGS) ex8_event_driven_SEIRS.cpp $(INCLUDE) $(LDFLAGS) -o ex8_event_driven_SEIRS

clean:
	rm -f test_network ex1_mass_action ex2_percolation ex3_chain_binomial ex4_dynamic_net ex5_diff_eq ex6_network_diff_eq ex7_gillespie_network_SEIRS
