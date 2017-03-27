SRCDIR = src
BINDIR = bin
LIBDIR = obj
DATADIR = data
TYPE ?= -O3
TARGET=lib/libgdrift++.a

# CPPFLAGS = $(TYPE) -Wall -I./includes/ -L./lib/ -std=c++0x -lrt
### Notar el -pthread para g++ 5.1 (no era necesario en el 4.6, bastaba con -std=c++0x)
CPPFLAGS = $(TYPE) -Wall -I./include/ -L./obj/ -std=c++0x -lrt

CPP = g++

#CREATED_OBJECTS = obj/NanoTimer.o obj/VirtualSequence.o obj/Bitset.o obj/Reference.o obj/Pool.o obj/Individual.o obj/Population.o obj/Event.o obj/EventList.o obj/Simulator.o obj/Model.o obj/Sample.o obj/Statistics.o

CREATED_OBJECTS = NanoTimer.o VirtualSequence.o Bitset.o Reference.o Pool.o Individual.o Population.o Event.o EventList.o Simulator.o Model.o Sample.o Statistics.o

OBJECTS = $(CREATED_OBJECTS) 
OBJECTS_LOC = $(OBJECTS:%.o=$(LIBDIR)/%.o)

.PHONY: all clean

$(TARGET): $(OBJECTS)
		ar rvs $(TARGET) $(OBJECTS_LOC)
		
#all: 

%.o: 
	make -C src $@
	
#%: $(OBJECTS)
#	$(CPP) $(OBJECTS_LOC) $(CPPFLAGS) -o $(SRCDIR)/$@.cpp
	
#%: obj/*.o
#	ar rvs $(TARGET) $^
	
clean:
	rm -f obj/*.o $(TARGET)
	
