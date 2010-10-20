
all: tmp_research

tmp_research: tmp_research.cpp
	g++ -I ../src ../src/*.o  tmp_research.cpp -o tmp_research

epi_sim: epi_sim.cpp MultiSeason_Sim.h
	g++ -O2 -I ../src ../src/*.o MultiSeason_Sim.h epi_sim.cpp -o epi_sim

