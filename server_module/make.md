# Compiler and flags
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support -I../map_module -I../game_module 

# Libraries - paths for libcs50.a and support.a
LIBS = ../libcs50/libcs50.a ../support/support.a 

# Executable
EXE = server

# removed source files 
# removed objs  
# Source files
SRCS = server.c ../map_module/map.c ../game_module/game.c
OBJS = server.o ../map_module/map.o ../game_module/game.o

# Build the executable
$(EXE): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)

# # Build libcs50 library
# libcs50/libcs50.a:
# 	$(MAKE) -C libcs50

# # Build support library
# support/support.a:
# 	$(MAKE) -C support

# Compile server.c
server.o: server.c ../game_module/game.h ../map_module/map.h server.h
	$(CC) $(CFLAGS) -c server.c

# # Compile game.o
# game.o: game_module/game.c game_module/game.h
# 	$(CC) $(CFLAGS) -c game_module/game.c -o game.o

# # Compile map.o
# map.o: map_module/map.c map_module/map.h
# 	$(CC) $(CFLAGS) -c map_module/map.c -o map.o

# Clean up generated files
clean:
	rm -f $(EXE) $(OBJS)
