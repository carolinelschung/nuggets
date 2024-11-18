# Makefile for CS50 Lab3 
#   Builds and compiles all the data structures.
#
# Caroline Chung, November 2024

# uncomment the following to turn on verbose memory logging
# (and run `make clean; make` whenever you change this)
# TESTING=-DMEMTEST

# Our Make program and its flags
MAKE = make TESTING=$(TESTING)

# recursively make in each subdirectory
all:
	$(MAKE) --directory=libcs50
	$(MAKE) --directory=support
	$(MAKE) --directory=map_module
	$(MAKE) --directory=game_module
	$(MAKE) --directory=server_module
	$(MAKE) --directory=client_module

# 'phony' targets are helpful but do not create any file by that name
.PHONY: clean

# to clean up all derived files
clean:
	rm -f *~
	make --directory=libcs50 clean
	make --directory=support clean
	make --directory=map_module clean
	make --directory=game_module clean
	make --directory=server_module clean
	make --directory=client_module clean
