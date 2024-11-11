# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -Ilibcs50 -Igame_module

# Libraries - libcs50.a path
LIBS = libcs50/libcs50.a

# Executable
EXE = server

# Source files
SRCS = server.c game_module/game.c
OBJS = server.o game.o

# Build the executable
$(EXE): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

# Build libcs50 library
$(LIBS):
	$(MAKE) -C libcs50

# Compile server.c
server.o: server.c game_module/game.h server.h
	$(CC) $(CFLAGS) -c server.c

# Compile game.c
game.o: game_module/game.c game_module/game.h
	$(CC) $(CFLAGS) -c game_module/game.c -o game.o

# Clean up generated files
clean:
	rm -f $(EXE) $(OBJS)
	$(MAKE) -C libcs50 clean
