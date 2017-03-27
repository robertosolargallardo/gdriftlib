SRCDIR = src
BINDIR = bin
LIBDIR = obj
TARGET=lib/libgdrift++.a

### Notar el -pthread para g++ 5.1 (no era necesario en el 4.6, bastaba con -std=c++0x)
CPPFLAGS = -Wall -I./include/ -L./obj/ -std=c++0x -lrt -O3

CPP = g++

CREATED_OBJECTS = NanoTimer.o VirtualSequence.o Bitset.o Reference.o Pool.o Individual.o Population.o Event.o EventList.o Simulator.o Model.o Sample.o Statistics.o

OBJECTS = $(CREATED_OBJECTS) 
OBJECTS_LOC = $(OBJECTS:%.o=$(LIBDIR)/%.o)

.PHONY: all clean

$(TARGET): $(OBJECTS)
		ar rvs $(TARGET) $(OBJECTS_LOC)

%.o: 
	make -C src $@
	
clean:
	rm -f obj/*.o $(TARGET)
	
