# Makefile for 'client' module
#
# Paul Cherian, Febuary 2024
#
# structure adapted from David Kotz 'Bag' Makefile April 2021

# objects
L = ../support
M = ../modules
C= ../libcs50
LLIBS = ../libcs50/libcs50-given.a ../support/support.a
MODULES = ../modules/grid.o ../modules/player.o

# specify c compiler type and cflag lib
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$M -I$L -I$C
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# mark non-file targets as phony
.PHONY: all test valgrind clean

all: server 

../modules/player.o: ../modules/player.h
	$(MAKE) -C ../modules

server: server.o $(MODULES) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

../modules/grid.o: ../modules/grid.h
	$(MAKE) -C ../modules

# Add a rule to run your testing.sh script
test: client testing.sh
	bash -v testing.sh


# clean up the directory
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core


	