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
MODULES = ../modules/game.o ../modules/player.o ../modules/spectator.o ../modules/grid.o

# specify c compiler type and cflag lib
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$M -I$L -I$C
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# mark non-file targets as phony
.PHONY: all test valgrind clean

all: server 

server: server.o $(MODULES) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

../modules/player.o: ../modules/player.h ../modules/grid.h
	$(MAKE) -C ../modules

../modules/grid.o: ../modules/grid.h ../modules/mapchars.h
	$(MAKE) -C ../modules

../modules/game.o: ../modules/game.h ../modules/grid.h ../modules/player.h ../modules/spectator.h ../modules/mapchars.h
	$(MAKE) -C ../modules

# Add a rule to run your testing.sh script
test: client testing.sh
	bash -v testing.sh


# clean up the directory
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core


	