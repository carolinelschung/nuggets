# Makefile for 'game' module
# Caroline Chung, November 17, 2024

# Object files
OBJS = game.o
LIBS = ../libcs50/libcs50.a ../support/support.a ../map_module/map.o

# Uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

# Compilation flags
CFLAGS = -Wall -pedantic -std=c11 -g -ggdb $(TESTING) -I../libcs50 -I../support -I../map_module
CC = gcc
MAKE = make

# Target for the game object file
game.o: game.c game.h ../map_module/map.h ../libcs50/hashtable.h ../libcs50/mem.h ../support/message.h
	$(CC) $(CFLAGS) -c game.c -o game.o

# For memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Phony targets to avoid conflicts with files
.PHONY: test valgrind clean

# Clean up object files and temporary files
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core
