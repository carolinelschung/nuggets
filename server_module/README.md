# CS50 Fall 2024 Final Project Team 10
## Server Module

This directory has the files ```server.c``` and the header file ```server.h```. To start any game the server is to be run from inside this directory. It has the ```Makefile``` to compile the server and create the object and executable files.

#### Usage
To initialize the server, in other words, the game, we use the following command:
```c 
./server ../maps/<map_name>
```
If we want the logging outputs to be in a different file, we can alternately use this
```c 
./server 2>server.log ../maps/<map_name>
```
If a map is ```edges.txt``` then our command would be 
```c 
./server ../maps/edges.txt
```
or
```c 
./server 2>server.log ../maps/edges.txt
```
To view the current status of the server, we write this:
```c 
status
```
If we want to stop the server or the game, then we write
```c 
quit
```
which will exit out of the server and stop the game the message module.
When the number of remaining nuggets is zero, the game ends, hence the server also stops.