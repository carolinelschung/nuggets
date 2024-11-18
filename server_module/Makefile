# Makefile for 'server' module
# Joseph Quaratiello, November 2024

# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support -I../map_module -I../game_module

# Libraries and external dependencies
LIBS = ../libcs50/libcs50.a ../support/support.a

# Object files required by server
OBJS = server.o ../map_module/map.o ../game_module/game.o

# Executable name
EXE = server

# Default target
all: $(EXE)

# Build the server executable
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

# Compile server.o
server.o: server.c server.h ../game_module/game.h ../map_module/map.h
	$(CC) $(CFLAGS) -c server.c -o server.o

# Ensure other modules are built
../map_module/map.o:
	$(MAKE) -C ../map_module

../game_module/game.o:
	$(MAKE) -C ../game_module

../libcs50/libcs50.a:
	$(MAKE) -C ../libcs50

../support/support.a:
	$(MAKE) -C ../support

# Phony targets
.PHONY: clean all

# For memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# Clean up generated files
clean:
	rm -f server.o $(EXE)
	$(MAKE) -C ../map_module clean
	$(MAKE) -C ../game_module clean
	$(MAKE) -C ../libcs50 clean
	$(MAKE) -C ../support clean
	rm -f *~ *.log
	
