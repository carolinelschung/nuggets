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
#include "support/message.h"
#include "libcs50/mem.h"


// Function prototypes
FILE* parseArgs(int argc, char* argv[], int* seed);
bool handleInput(void* arg);
bool handleMessage(void* arg, const addr_t from, const char* buf);

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

  // Awake messaging systrem and announce port
  game->port = message_init(stderr);
  if (game->port == 0) {
      fprintf(stderr, "Error: Failed to initialize messaging system.\n");
      mem_free(game);
      return 1;
  }

  bool success = message_loop(NULL, 0, NULL, handleInput, handleMessage);

  if (!success) {
    fprintf(stderr, "Error in message loop\n");
  }

  // Clean up after the loop ends
  mem_free(game);
  return 0;
  
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

// Helper Functions
bool handleInput(void* arg) {
    char input[100];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (strcmp(input, "quit\n") == 0) {
            printf("Server shutting down.\n");
            return true;  // Return true to exit the message loop
        } else if (strcmp(input, "status\n") == 0) {
            printf("Server status: running...\n");
            // Optionally provide more detailed game status info here
        }
    }
    return false;  // Return false to keep the loop running
}

bool handleMessage(void* arg, const addr_t from, const char* buf) {
    printf("Received message from %s: %s\n", message_stringAddr(from), buf);

    if (strncmp(buf, "PLAY ", 5) == 0) {
        const char* playerName = buf + 5;  // Extract player name
        if (strlen(playerName) > 0) {
            // Add player to the game
            printf("New player joining: %s\n", playerName);
            // Send OK message back to player with assigned letter
            char response[10];
            snprintf(response, sizeof(response), "OK A"); // Assign 'A' as example
            message_send(from, response);
        } else {
            message_send(from, "QUIT Sorry - you must provide a player's name.");
        }
    } else if (strcmp(buf, "SPECTATE") == 0) {
        printf("Spectator joining.\n");
        // Add or replace current spectator
        message_send(from, "GRID nrows ncols");  // Send initial grid size
        message_send(from, "DISPLAY\n...");      // Send current game state
    } else if (strncmp(buf, "KEY ", 4) == 0) {
        char key = buf[4];
        printf("Key received from player: %c\n", key);
        // Process the key command (e.g., player movement)
    } else {
        message_send(from, "ERROR Unrecognized command");
    }

    return false;  // Return false to keep the loop running
}




