/* 
 * server.c - CS50 Nuggets game module
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

int main(int argc, char* argv[])
{

  int seed, gold, minPiles, maxPiles;
  bool plain = false;
  // Parse args and open map file
  FILE* mapFile = parseArgs(argc, argv, &seed, &gold, &minPiles, &maxPiles, &plain);

  // initialize the game
  game_t* game = game_init(mapFile, seed, gold, minPiles, maxPiles, plain);
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
  message_done();
  return 0;
  
}


// Function to parse command-line arguments, validate them, and open the map file
FILE* parseArgs(int argc, char* argv[], int* seed, int* gold, int* minPiles, int* maxPiles, bool* plain) {
    *gold = 250;  // Default values
    *minPiles = 10;
    *maxPiles = 30;
    *seed = 0;

    int option;
    struct option options[] = {
      {"gold", required_argument, 0, 'g'},
      {"minpiles", required_argument, 0, 'm'},
      {"maxpiles", required_argument, 0, 'x'},
      {"seed", required_argument, 0, 's'},
      {"plain", no_argument, 0, 'p'},
      {0, 0, 0, 0}
    };

    while ((option = getopt_long(argc, argv, "g:m:x:s:p", options, NULL)) != -1) {
      switch (option) {
        case 'g':
          *gold = atoi(optarg);
          break;
        case 'm':
          *minPiles = atoi(optarg);
          break;
        case 'x':
          *maxPiles = atoi(optarg);
          break;
        case 's':
          *seed = atoi(optarg);
          break;
        case 'p':
          *plain = true;
          break;

        case '?':
            fprintf(stderr, "Usage: %s [--gold 500] [--minpiles 15] [--maxpiles 40] [--seed 123] map.txt\n", argv[0]);
            exit(1);
      }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s map.txt [options]\n", argv[0]);
        exit(1);
    }

    const char* mapFilename = argv[optind];
    FILE* mapFile = fopen(mapFilename, "r");
    if (mapFile == NULL) {
        fprintf(stderr, "Failed to open %s\n", mapFilename);
        exit(1);
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

    // Check whether the incoming message is from a player or a spectator
    // if (game->activePlayersCount < 26) {

    // }
    // else {

    // }
    if (strncmp(buf, "PLAY ", 5) == 0) {
        if (game->activePlayersCount < 26) {
            const char* playerName = buf + 5;  // Extract player name
          
            if (strlen(playerName) > 0) {
                char acceptedName[MAX_NAME_LENGTH + 1];
                acceptedName[0] = '\0';
                strncpy(acceptedName, playerName, MAX_NAME_LENGTH);
        
                // Ensure null termination
                acceptedName[MAX_NAME_LENGTH] = '\0';
                
                // Iterate through the copied string
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
                char response[5];
                response[0] = '\0';
                sprintf(response, "OK %c", player->playerLetter);
                message_send(from, response);
                char result[50];
                result[0] = '\0';
                sprintf(result, "GRID %d %d", game->mapHeight, game->mapWidth);
                message_send(from, result);  // Send initial grid size
                char gold[12];
                gold[0] = '\0';
                sprintf(gold, "GOLD %d %d %d", 0, 0, game->goldRemaining);
                message_send(from, gold);
                char first_part[] = "DISPLAY\n";
                char* map = map_decode(player->playerMap, game);
                
                char message[message_MaxBytes];
                message[0] = '\0';
                snprintf(message, message_MaxBytes,"%s%s", first_part, map);
                
                message_send(from, message);
                mem_free(map);

                
            } 
            else {
                message_send(from, "QUIT Sorry - you must provide a player's name.");
            }
        }
        else {
            message_send(from, "QUIT Game is full: no more players can join.");
        }
       
    } 
    else if (strcmp(buf, "SPECTATE") == 0) {
        if (game->hasSpectator) {
            message_send(game->spectatorAddress, "QUIT You have been replaced by a new spectator");
            // the server needs to forget this spectator
        }
        else {
            game->hasSpectator = true;
        }
        game->spectatorAddress = from;

        printf("Spectator joining.\n");
        char result[50];
        result[0] = '\0';
        sprintf(result, "GRID %d %d", game->mapHeight, game->mapWidth);
        message_send(from, result);  // Send initial grid size
        char gold[12];
        gold[0] = '\0';
        sprintf(gold, "GOLD %d %d %d", 0, 0, game->goldRemaining);
        message_send(from, gold);
        // Send current game state
        char first_part[] = "DISPLAY\n";
        char* map = map_decode(game->map, game);
        char message[message_MaxBytes];
        message[0] = '\0';
        snprintf(message, message_MaxBytes,"%s%s", first_part, map);
        message_send(from, message);
        mem_free(map);
    } 

    else if (strncmp(buf, "KEY ", 4) == 0) {
        char key = buf[4];
        printf("Key received from player: %c\n", key);
        /* Process the key command (e.g., player movement)*/

        // Array of valid controls
        char valid_chars[] = "QhljkyubnHLJKYUBN";

        //Checking if it's a valid key;
        if (strchr(valid_chars, key)) {
          printf("The keystroke is valid\n");
          //fflush(stderr);
          if (key == 'Q' || key == 'q') {
              if (message_eqAddr(from, game->spectatorAddress)) {
                  message_send(from, "QUIT Thanks for watching");
              }
              else {
                message_send(from, "QUIT Thanks for playing");
                player_t* quittingPlayer = hashtable_find(game->players, message_stringAddr(from));
                if (quittingPlayer != NULL) {
                  int indexToLeave = quittingPlayer->yPosition * game->mapWidth + quittingPlayer->xPosition;
                  if (!game->plain) {
                    int indexToLeave = quittingPlayer->yPosition * game->mapWidth + quittingPlayer->xPosition;
                    game->map[indexToLeave] = '*';

                    char key[12];
                    key[0] = '\0';
                    snprintf(key, sizeof(key), "%d", indexToLeave);

                    int* goldCapturedPtr = malloc(sizeof(int));
                    if (goldCapturedPtr == NULL) {
                        fprintf(stderr, "Error: Failed to allocate memory for goldCaptured.\n");
                        return false;
                    }
                    *goldCapturedPtr = quittingPlayer->goldCaptured;
                    game->goldRemaining += quittingPlayer->goldCaptured;

                    quittingPlayer->goldCaptured = 0; //drops the gold

                    hashtable_insert(game->goldPileAmounts, key, goldCapturedPtr);
                  } else {
                    game->map[indexToLeave] = '.';
                  }
                  for (int i = 0; i < MaxPlayers; i++) {
                    if (message_eqAddr(game->activePlayers[i], from)) {
                      game->activePlayers[i] = message_noAddr();
                      break;
                    }
                  }
                }
              }
            }
            // else {
                
            // }
            // need a boolean here to check whether the move was valid or not
            // It fails inside the helper function of game_playerMove
            bool valid = game_playerMove(from, game, key);
            /*if new_gold_remaining is not equal to num_gold_remaining, 
            we wanna send gold message to all the clients as well in the loop */
            // printf("{%d}\n", valid);
            // fflush(stdout);
            
            if (valid) {
                
                for (int i = 0; i < MaxPlayers; i++) {
                    
                    if ((message_isAddr(game->activePlayers[i])) ) {

                        player_t* player = hashtable_find(game->players, message_stringAddr((game->activePlayers[i])));
                     
                        char first_part[] = "DISPLAY\n";
                        char* map = map_decode(player->playerMap, game);
                        // printf("map declared\n");
                        //fflush(stdout);
                        char message[message_MaxBytes];
                        message[0] = '\0';
                        // printf("Message Declared");
                        // fflush(stdout);
                        snprintf(message, message_MaxBytes,"%s%s", first_part, map);
                        // printf("Message Assigned");
                        // fflush(stdout);
                       
                        message_send((game->activePlayers[i]), message);
                        char gold[12];
                        gold[0] = '\0';
                        sprintf(gold, "GOLD %d %d %d", player->goldJustCaptured, player->goldCaptured, game->goldRemaining);
                        message_send(game->activePlayers[i], gold);
                        
                        if (game->goldRemaining == 0) {
                            // send message to all players and the spectator printing out the result
                            char end_part[] = "QUIT GAME OVER:\n";
                            char end_message[message_MaxBytes];
                            end_message[0] = '\0';
                            char* finalScores = game_getFinalScores(game);
                            snprintf(end_message, message_MaxBytes,"%s%s", end_part, finalScores);
                            message_send(game->activePlayers[i], end_message);
                            mem_free(finalScores);
                        }
                        mem_free(map);

                    } 
                }
                char first_part[] = "DISPLAY\n";
                char* map = map_decode(game->map, game);
                char message[message_MaxBytes];
                message[0] = '\0';
                snprintf(message, message_MaxBytes,"%s%s", first_part, map);
                if(game->hasSpectator){
                  message_send(game->spectatorAddress, message);
                }
                char gold[12];
                gold[0] = '\0';
                sprintf(gold, "GOLD %d %d %d", 0, 0, game->goldRemaining);
                if(game->hasSpectator){
                  message_send(game->spectatorAddress, gold);
                }
                mem_free(map);

                if (game->goldRemaining == 0) {
                    // send message to the spectator printing out the result
                    char end_part[] = "QUIT GAME OVER:\n";
                    char end_message[message_MaxBytes];
                    char* finalScores = game_getFinalScores(game);
                    snprintf(end_message, message_MaxBytes,"%s%s", end_part, finalScores);
                    if(game->hasSpectator){
                      message_send(game->spectatorAddress, end_message);
                    }
                    mem_free(finalScores);
                    
                }

            }
            

        } 
        else {
            message_send(from, "ERROR : Not a valid input");
        }
    } 
    else {
        message_send(from, "ERROR Unrecognized command");
    }

    return false;  // Return false to keep the loop running
}

