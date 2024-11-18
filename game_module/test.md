# Makefile for 'game' module
# Caroline Chung, November 17, 2024

# Object files
OBJS = game.o
LIBS = ../libcs50/libcs50.a ../support/support.a

# Uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

# Compilation flags
CFLAGS = -Wall -pedantic -std=c11 -g -ggdb $(TESTING) -I../libcs50 -I../support -I../map_module  
CC = gcc
MAKE = make

# For memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Target for the main game program

# Dependencies
game.o: game.c game.h
	$(CC) $(CFLAGS) -c game.c -o game.o

# Phony targets to avoid conflicts with files
.PHONY: test valgrind clean

# Clean up object files and the game program
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core