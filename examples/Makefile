CFLAGS=--ansi --pedantic -O3
INCLUDE= -I../src/
#LDFLAGS= -L../src/ -lsim 
LDFLAGS=  ../src/*.o

all: test_network  example2 EX1_mass_action

test_network: test_network.cpp 
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) test_network.cpp -o test_network  

example2: example2.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) example2.cpp -o example2

EX1_mass_action: EX1_mass_action.cpp
	g++ $(CFLAGS) $(INCLUDE) $(LDFLAGS) EX1_mass_action.cpp -o EX1_mass_action



clean:
	rm -f test_network 

