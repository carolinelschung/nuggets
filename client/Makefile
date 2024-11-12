# Makefile for 'client' program
# Caroline Chung, November 12, 2024

# Object files
OBJS = client.o 
LIBS = -lncurses ../support/support.a

# Uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

# Compilation flags
CFLAGS = -Wall -pedantic -std=c11 -g -ggdb $(TESTING) -I../support
CC = gcc
MAKE = make

# For memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all


# Target for the main client program
client: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Dependencies
client.o: client.c ../support/message.h ../support/log.h

# Phony targets to avoid conflicts with files
.PHONY: test valgrind clean

# Clean up object files and the client program
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f client
	rm -f core