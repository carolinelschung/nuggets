# # Makefile for 'map' module
# # Caroline Chung, November 17, 2024

# # Object files
# OBJS = map.o
# LIBS = ../libcs50/libcs50.a 

# # Uncomment the following to turn on verbose memory logging
# # TESTING=-DMEMTEST

# # Compilation flags
# CFLAGS = -Wall -pedantic -std=c11 -g -ggdb $(TESTING) -I../libcs50 
# CC = gcc
# MAKE = make

# # For memory-leak tests
# VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# # Target for the map module
# map: $(OBJS)
# 	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# # Dependencies
# map.o: map.h

# # Phony targets to avoid conflicts with files
# .PHONY: test valgrind clean

# # Clean up object files and the map module program
# clean:
# 	rm -rf *.dSYM  # MacOS debugger info
# 	rm -f *~ *.o
# 	rm -f map
# 	rm -f core



# Makefile for 'map' module
#
# Anna Filyurina November 2024, based on the 'map' module 
# 

OBJS = map.o #../lib/mem.o 
LIBS = libcs50/libcs50.a support/support.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support
CC = gcc
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# map: $(OBJS)
# 	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# # map_module_test.o: map_module.h # ../lib/mem.h
# map.o: map.h
# #../lib/mem.o: ../lib/mem.h

map.o: map.c map.h
	$(CC) $(CFLAGS) -c map.c -o map.o


.PHONY: test valgrind clean

# expects a file `test.names` to exist; it can contain any text.
# test: map_module_test
# 	./map_module_test

# # expects a file `test.names` to exist; it can contain any text.
# valgrind: counterstest test.counters
# 	$(VALGRIND) ./counterstest < test.counters

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core