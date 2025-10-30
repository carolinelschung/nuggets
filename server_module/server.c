/* 
 * server.c - CS50 Nuggets server module, Team 10
 *
 * see server.h for more information.
 *
 * Joseph Quaratiello, Nafis Saadiq Bhuiyan, November 2024
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../game_module/game.h"
#include "server.h"
#include "../support/message.h"
#include "../libcs50/mem.h"
#include <ctype.h>
#include "../map_module/map.h"
#include <getopt.h>

#define MAX_NAME_LENGTH 50 // max number of chars in playerName
// Function prototypes
bool handleInput(void* arg);
bool handleMessage(void* arg, const addr_t from, const char* buf);
void updateAllPlayers(game_t* game);

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

  //game_test(game);

  bool success = message_loop(game, 0, NULL, handleInput, handleMessage);

  if (!success) {
    fprintf(stderr, "Error in message loop\n");
  }

  // Clean up after the loop ends
  game_delete(game);
  fclose(stdout);
  message_done();
  return 0;
  
}


// Function to parse command-line arguments, validate them, and open the map file
FILE* parseArgs(int argc, char* argv[], int* seed) {

    *seed = 0;  // Default seed (will use getpid() if not specified)

    // Validate positional arguments
    if (optind >= argc) {
        fprintf(stderr, "Usage: %s map.txt [seed] [options]\n", argv[0]);
        exit(1);
    }

    const char* mapFilename = argv[optind];
    FILE* mapFile = fopen(mapFilename, "r");
    if (mapFile == NULL) {
        fprintf(stderr, "Failed to open %s\n", mapFilename);
        exit(1);
    }

    // Check for an optional seed argument
    if (optind + 1 < argc) {
        *seed = atoi(argv[optind + 1]);
        if (*seed <= 0) {
            fprintf(stderr, "Seed must be a positive integer.\n");
            fclose(mapFile);
            exit(1);
        }
    } else {
        *seed = getpid();  // Default to getpid() if no seed provided
    }

    return mapFile;
}



// Helper Functions
bool handleInput(void* arg) 
{
    char input[100];
    input[0] = '\0';
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

bool handleMessage(void* arg, const addr_t from, const char* buf) 
{
    game_t* game = (game_t*) arg;

    printf("Received message from %s: %s\n", message_stringAddr(from), buf);

    if (strncmp(buf, "PLAY ", 5) == 0) {
        // Handle player joining
        if (game->activePlayersCount < 26) {
            const char* playerName = buf + 5;
            if (strlen(playerName) > 0) {
                char acceptedName[MAX_NAME_LENGTH + 1];
                strncpy(acceptedName, playerName, MAX_NAME_LENGTH);
                acceptedName[MAX_NAME_LENGTH] = '\0'; // Ensure null termination

                for (int i = 0; acceptedName[i] != '\0'; i++) {
                    if (!isgraph(acceptedName[i]) && !isblank(acceptedName[i])) {
                        acceptedName[i] = '_';
                    }
                }

                printf("New player joining: %s\n", acceptedName);
                player_t* player = game_playerInit(game, from, acceptedName);
                if (player == NULL) {
                    printf("Player not initialized properly\n");
                    fflush(stdout);
                }

                // Send acknowledgment and initial game data
                char response[5];
                sprintf(response, "OK %c", player->playerLetter);
                message_send(from, response);

                char result[50];
                sprintf(result, "GRID %d %d", game->mapHeight, game->mapWidth);
                message_send(from, result);

                char gold[12];
                sprintf(gold, "GOLD %d %d %d", 0, 0, game->goldRemaining);
                message_send(from, gold);

                char first_part[] = "DISPLAY\n";
                char* map = map_decode(player->playerMap, game);
                char message[message_MaxBytes];
                snprintf(message, sizeof(message), "%s%s", first_part, map);
                message_send(from, message);
                mem_free(map);

                // Update all players and the spectator
                updateAllPlayers(game);
            } else {
                message_send(from, "QUIT Sorry - you must provide a player's name.");
            }
        } else {
            message_send(from, "QUIT Game is full: no more players can join.");
        }
    } 
    else if (strcmp(buf, "SPECTATE") == 0) {
        // Handle spectator joining or replacing an existing spectator
        if (game->hasSpectator) {
            message_send(game->spectatorAddress, "QUIT You have been replaced by a new spectator");
        } else {
            game->hasSpectator = true;
        }
        game->spectatorAddress = from;

        printf("Spectator joining.\n");

        // Send initial grid dimensions
        char result[50];
        snprintf(result, sizeof(result), "GRID %d %d", game->mapHeight, game->mapWidth);
        message_send(from, result);

        // Send initial gold information
        char gold[12];
        snprintf(gold, sizeof(gold), "GOLD %d %d %d", 0, 0, game->goldRemaining);
        message_send(from, gold);

        // Send the current game state
        char first_part[] = "DISPLAY\n";
        char* map = map_decode(game->map, game);
        char message[message_MaxBytes];
        snprintf(message, sizeof(message), "%s%s", first_part, map);
        message_send(from, message);
        mem_free(map);
    } 
    else if (strncmp(buf, "KEY ", 4) == 0) {
        // Handle player movement or quitting
        char key = buf[4];
        printf("Key received from player: %c\n", key);

        if (key == 'Q' || key == 'q') {
            // Handle player quitting
            if (message_eqAddr(from, game->spectatorAddress)) {
                message_send(from, "QUIT Thanks for watching");
                game->hasSpectator = false;
            } else {
                message_send(from, "QUIT Thanks for playing");
                player_t* quittingPlayer = hashtable_find(game->players, message_stringAddr(from));
                if (quittingPlayer != NULL) {
                    int indexToLeave = quittingPlayer->yPosition * game->mapWidth + quittingPlayer->xPosition;
                    char originalTile = game->mapWithNoPlayers[indexToLeave];
                    game->map[indexToLeave] = originalTile;
                }
            }

            // Update all players and the spectator
            updateAllPlayers(game);
        } else {
            // Process valid movement keys
            char valid_chars[] = "QhljkyubnHLJKYUBN";
            if (strchr(valid_chars, key)) {
                if (game_playerMove(from, game, key)) {
                    // Movement succeeded, update all players and the spectator
                    updateAllPlayers(game);

                    // Check if game is over
                    if (game->goldRemaining == 0) {
                        char end_part[] = "QUIT GAME OVER:\n";
                        char* finalScores = game_getFinalScores(game);

                        // Notify all players
                        for (int i = 0; i < MaxPlayers; i++) {
                            if (message_isAddr(game->activePlayers[i])) {
                                char end_message[message_MaxBytes];
                                snprintf(end_message, sizeof(end_message), "%s%s", end_part, finalScores);
                                message_send(game->activePlayers[i], end_message);
                            }
                        }

                        // Notify spectator if present
                        if (game->hasSpectator) {
                            char end_message[message_MaxBytes];
                            snprintf(end_message, sizeof(end_message), "%s%s", end_part, finalScores);
                            message_send(game->spectatorAddress, end_message);
                        }

                        mem_free(finalScores);
                        return true; // Exit the game loop
                    }
                }
            } else {
                message_send(from, "ERROR Not a valid input");
            }
        }
    } else {
        // Handle unrecognized command
        message_send(from, "ERROR Unrecognized command");
    }

    return false; // Keep the loop running
}


void updateAllPlayers(game_t* game) {
    for (int i = 0; i < MaxPlayers; i++) {
        if (message_isAddr(game->activePlayers[i])) {
            player_t* player = hashtable_find(game->players, message_stringAddr(game->activePlayers[i]));
            if (player != NULL) {
                // Update the player's visible map
                char* visibleMap = mem_malloc(strlen(game->map) + 1);
                memset(visibleMap, 0, strlen(game->map) + 1);
                map_get_visible(player->xPosition, player->yPosition, game->map, visibleMap, game->mapWidth, game->mapHeight);
                map_merge(player->playerMap, visibleMap, game->mapWidth, game->mapHeight);
                mem_free(visibleMap);

                // Send the updated map to the player
                char first_part[] = "DISPLAY\n";
                char* map = map_decode(player->playerMap, game);
                char message[message_MaxBytes];
                memset(message, 0, sizeof(message));
                snprintf(message, message_MaxBytes, "%s%s", first_part, map);
                message_send(game->activePlayers[i], message);
                mem_free(map);

                // Send updated gold info
                char goldInfo[50];
                snprintf(goldInfo, sizeof(goldInfo), "GOLD %d %d %d", player->goldJustCaptured, player->goldCaptured, game->goldRemaining);
                message_send(game->activePlayers[i], goldInfo);
            }
        }
    }
}


