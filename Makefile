# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -Ilibcs50 -Igame_module -Isupport

# Libraries - paths for libcs50.a and support.a
LIBS = libcs50/libcs50.a support/support.a

# Executable
EXE = server

# Source files
SRCS = server.c game_module/game.c map_module/map.c
OBJS = server.o game.o map.o

# Build the executable
$(EXE): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

# Build libcs50 library
libcs50/libcs50.a:
	$(MAKE) -C libcs50

# Build support library
support/support.a:
	$(MAKE) -C support

# Compile server.c
server.o: server.c game_module/game.h server.h
	$(CC) $(CFLAGS) -c server.c

# Compile game.c
game.o: game_module/game.c game_module/game.h
	$(CC) $(CFLAGS) -c game_module/game.c -o game.o

# Compile map.c
map.o: map_module/map.c map_module/map.h
	$(CC) $(CFLAGS) -c map_module/map.c -o map.o

# Clean up generated files
clean:
	rm -f $(EXE) $(OBJS)
	$(MAKE) -C libcs50 clean
	$(MAKE) -C support clean
