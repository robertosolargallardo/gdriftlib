TARGET=test
SRC=test.cc
LIBS=-L../../lib/
INCLUDE=-I../../src/
CXX=g++
CXXFLAGS=-std=c++11 -lgdrift++ -g -ggdb

$(TARGET):$(SRC)
			 $(CXX) $< -o $@ $(LIBS) $(INCLUDE) $(CXXFLAGS)

clean:
		${RM} $(TARGET)
