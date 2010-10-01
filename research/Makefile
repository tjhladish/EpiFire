all: tmp_research msms_sim

tmp_research: tmp_research.cpp
	g++ -I ../src ../src/*.o  tmp_research.cpp -o tmp_research

msms_sim: msms_simulator.cpp MSMS_Sim.h
	g++ -g -I ../src ../src/*.o  MSMS_Sim.h msms_simulator.cpp -o msms_sim

