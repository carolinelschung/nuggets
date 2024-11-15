# Makefile for 'counters' module
#
# Anna Filyurina October 2024, based on the 'counters' module 
# 

OBJS = map_module_test.o map_module.o #../lib/mem.o 
LIBS =

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

map_module_test: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

map_module_test.o: map_module.h # ../lib/mem.h
map_module.o: map_module.h
#../lib/mem.o: ../lib/mem.h


.PHONY: test valgrind clean

# expects a file `test.names` to exist; it can contain any text.
test: map_module_test
	./map_module_test

# # expects a file `test.names` to exist; it can contain any text.
# valgrind: counterstest test.counters
# 	$(VALGRIND) ./counterstest < test.counters

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f map_module_test
	rm -f core
