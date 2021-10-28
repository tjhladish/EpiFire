-include local.mk

CC := g++
CFLAGS := -c -std=c++14 -O2 -Wall --pedantic -fPIC
#CFLAGS=-c -std=c++11 -g -O0
SOURCES := Network.cpp Utility.cpp
INCLUDE := -I./
LDFLAGS :=


OBJECTS=$(SOURCES:.cpp=.o)
LIB=libepifire.a

all: $(SOURCES) $(LIB)

ARCHIVE ?= $(AR) -rv

$(LIB): $(OBJECTS)
	$(ARCHIVE) $(LIB) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm $(OBJECTS) $(LIB)
