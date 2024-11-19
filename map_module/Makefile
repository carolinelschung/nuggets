# Makefile for 'map' module (Team 10)
#
# Anna Filyurina November 2024, based on the 'map' module 
# 

OBJS = map.o #../lib/mem.o 
LIBS = ../libcs50/libcs50.a ../support/support.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support
CC = gcc
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

map.o: map.c map.h
	$(CC) $(CFLAGS) -c map.c -o map.o

.PHONY: test valgrind clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core