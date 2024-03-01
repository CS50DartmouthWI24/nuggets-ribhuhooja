# Makefile for 'server' module
#
# Sam Starrs, Febuary 2024
#
# structure adapted from David Kotz 'Bag' Makefile April 2021

# objects
L = ./support
# C = 

# specify c compiler type and cflag lib
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
CC = gcc
MAKE = make

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# mark non-file targets as phony
.PHONY: all test valgrind clean

all: server

# message server miniserver miniclient

miniclient.o: ./support/miniclient.c
	$(CC) $(CFLAGS) -c $< -o $@

miniserver.o: ./support/miniserver.c
	$(CC) $(CFLAGS) -c $< -o $@

message.o: ./support/message.c
	$(CC) $(CFLAGS) -c $< -o $@ 

message: message.o
	$(CC) -o $@ $^ $(CFLAGS) -L$L -l:support.a

miniclient: miniclient.o
	$(CC) -o $@ $^ $(CFLAGS) -L$L -l:support.a

miniserver: miniserver.o
	$(CC) -o $@ $^ $(CFLAGS) -L$L -l:support.a

server: server.o
	$(CC) -o $@ $^ $(CFLAGS) -L$L -l:support.a


# Add a rule to run your testing.sh script
test: client testing.sh
	bash -v testing.sh


# clean up the directory
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core


	