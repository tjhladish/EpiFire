CFLAGS=--ansi --pedantic -O3
all: test_network epi_sim chain_bin 
BINDIR=../bin/

epi_sim: epi_sim.cpp Multi_SeasonSim.h
	g++ $(CFLAGS) -I ../src ../src/*.o -o epi_sim epi_sim.cpp Multi_SeasonSim.h

clean:
	rm -f epi_sim 

