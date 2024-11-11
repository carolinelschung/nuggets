/* 
 * server.c - CS50 Nuggets game module
 *
 * see server.h for more information.
 *
 * Joseph Quaratiello, November 2024
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "game_module/game.h"
#include "server.h"


int main(int argc, char* argv[])
{

  int seed;

  // Parse args and open map file
  FILE* mapFile = parseArgs(argc, argv, &seed);

  // initialize the game
  game_t* game = game_init(mapFile, seed);
  if (game == NULL) {
    fprintf(stderr, "Error: Failed to initialize game\n");
    return 1;
  }

  game_print(game);

  printf("Game initialized successfully with map.\n");
  printf("Waiting for players to join...\n");

  //TODO: Set up server accept connections/handle game logic

  
}


// Function to parse command-line arguments, validate them, and open the map file
FILE* parseArgs(int argc, char* argv[], int* seed) 
{
    // Check for the correct number of arguments
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s map.txt [seed]\n", argv[0]);
        exit(1);
    }

    // Open the map file
    const char* mapFilename = argv[1];
    FILE* mapFile = fopen(mapFilename, "r");
    if (mapFile == NULL) {
        fprintf(stderr, "Failed to open %s\n", mapFilename);
    }

    // Parse the optional seed argument, default to 0 if not provided
    *seed = (argc == 3) ? atoi(argv[2]) : 0;
    if (*seed < 0) {
        fprintf(stderr, "Seed must be a positive int\n");
    }

    return mapFile; // Return the opened map file
}




