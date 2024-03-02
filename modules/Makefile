# A makefile to compile all the modules
#
# Ribhu Hooja, February 2024

CC = gcc
CCDIRS = -I../support -I../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(CCDIRS)

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

LIBS = 
LLIBS = ../support/support.a ../libcs50/libcs50-given.a

all: grid.o player.o spectator.o game.o

.PHONY: all clean gridtest

gridtest: gridtest.o grid.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@
	$(VALGRIND) ./$@

grid.o: grid.h
player.o: player.h grid.h
spectator.o: spectator.h
game.o: game.h spectator.h player.h grid.h mapchars.h

gridtest.o: grid.h


../support/support.a:
	$(MAKE) -C ../support

clean:
	rm -f *.o
	rm -f gridtest
	rm -f visibilitytest
