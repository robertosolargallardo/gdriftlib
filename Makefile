#TARGET=test
TARGET=lib/libgdrift++.a
TEST=test
CXX=g++
CXXFLAGS=-std=c++11 -Wall -g -ggdb -O3 

OBJS=obj/NanoTimer.o obj/VirtualSequence.o obj/Bitset.o obj/Reference.o obj/Pool.o obj/Individual.o obj/Population.o obj/Event.o obj/EventList.o obj/Simulator.o obj/Model.o obj/Sample.o obj/Statistics.o

$(TARGET):$(OBJS)
		ar rvs $(TARGET) $^

obj/NanoTimer.o:src/NanoTimer.cc src/NanoTimer.h
				 $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/VirtualSequence.o:src/VirtualSequence.cc src/VirtualSequence.h
				 $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Bitset.o:src/Bitset.cc src/Bitset.h
				 $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Reference.o:src/Reference.cc src/Reference.h
					 $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Pool.o:src/Pool.cc src/Pool.h
			  $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Individual.o:src/Individual.cc src/Individual.h
					  $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Population.o:src/Population.cc src/Population.h
					  $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Event.o:src/Event.cc src/Event.h
				$(CXX) -c $< -o $@ $(CXXFLAGS)
obj/EventList.o:src/EventList.cc src/EventList.h
					 $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Simulator.o:src/Simulator.cc src/Simulator.h
					 $(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Model.o:src/Model.cc src/Model.h
				$(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Sample.o:src/Sample.cc src/Sample.h
				$(CXX) -c $< -o $@ $(CXXFLAGS)
obj/Statistics.o:src/Statistics.cc src/Statistics.h
				$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
		${RM} $(TARGET) $(OBJS)
