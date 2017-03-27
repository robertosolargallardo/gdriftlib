SRCDIR = ./
BINDIR = ../bin
LIBDIR = ../obj
DATADIR = ../data
TYPE ?= -O3
CPPFLAGS = $(TYPE) -Wall -I../include/ -L../obj/ -std=c++0x
CFLAGS = $(TYPE) -Wall -I../include/ -L../obj/

CPP = g++

%.o: %.cc 
	$(CPP) $(CPPFLAGS) -c $< -o $(LIBDIR)/$@


%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $(LIBDIR)/$@


%.o: %.c
	$(CPP) $(CFLAGS) -c $< -o $(LIBDIR)/$@

	
