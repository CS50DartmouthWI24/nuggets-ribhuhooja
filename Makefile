# Makefile for CS50 Nuggets
#
# Ribhu Hooja, March 2024
# Adapted from the CS50 TSE Makedile

L = libcs50
.PHONY: all clean

############## default: make all libs and programs ##########
# Note - this does not make the libcs50 directory, because
# we use the pre-provided libcs50-given.a archive instead
# of compiling one from source files. All the other files we use
# can be compiled.
all: 
	make -C support
	make -C modules
	make -C server
	make -C client

############## clean  ##########
clean:
	rm -f *~
	make -C support clean
	make -C modules clean
	make -C server clean
	make -C client clean
