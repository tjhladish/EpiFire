CC=g++ 
#CFLAGS=-c -O3 -Wall --pedantic
CFLAGS=-c -g
SOURCES= Network.cpp Utility.cpp 
INCLUDE= -I./
LDFLAGS= 


OBJECTS=$(SOURCES:.cpp=.o)
LIB=libsim.a

all: $(SOURCES) $(LIB) epi_sim
	
$(LIB): $(OBJECTS) 
	$(AR) -rv $(LIB) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm $(OBJECTS) $(LIB)

epi_sim: epi_sim.cpp
	$(CC) -g $(INCLUDE) epi_sim.cpp Network.cpp Utility.cpp -o epi_sim
