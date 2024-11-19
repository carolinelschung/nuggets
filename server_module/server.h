/* 
 * server.h - CS50 Nuggets server module, Team 10
 *
 * Runs the game, handles messaging and calls everything required for the game
 *
 * Joseph Quaratiello, Nafis Saadiq Bhuiyan, November 2024
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include "../game_module/game.h"  // Adjust the path to game.h as needed


/**************** Function Prototypes ****************/

/**
 * Parses command-line arguments, validates them, and opens the map file.
 * @param argc the argument count from main
 * @param argv the argument vector from main
 * @param seed pointer to an integer where the seed will be stored
 * @return FILE pointer to the opened map file, or NULL if failed
 */
FILE* parseArgs(int argc, char* argv[], int* seed, int* gold, int* minPiles, int* maxPiles, bool* plain);

/**
 * Prints the details of the initialized game for verification purposes.
 * @param game pointer to the game structure to be printed
 */
void printGame(const game_t* game);

#endif // SERVER_H
