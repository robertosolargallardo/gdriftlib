TARGET=test
SRC=test.cc
LIBS=-L../../lib/
INCLUDE=-I../../include/
CXX=g++
CXXFLAGS=-std=c++11 -lgdrift++ -fPIC -shared

$(TARGET):$(SRC)
			 $(CXX) $< -o $@ $(LIBS) $(INCLUDE) $(CXXFLAGS)

clean:
		${RM} $(TARGET)
