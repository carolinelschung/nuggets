/* 
 * game.h - header file for CS50 Nuggets game module (Team 10)
 *
 * A *game* module for the CS50 Nuggets game, responsible for managing 
 * the overall game state, including player and spectator data, map 
 * information, and gold distribution.
 *
 * This module initializes the game environment, handles player actions, 
 * and updates the game state based on client interactions. It supports 
 * random placement of gold piles, player positioning, and real-time 
 * updates for connected clients.
 *
 * Joseph Quaratiello, November 2024
 */

#ifndef __GAME_H
#define __GAME_H

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../support/message.h"

#define MaxPlayers 26

/**************** global types ****************/
typedef struct player {
    char* playerName;
    char playerLetter;
    char* playerMap;
    int goldJustCaptured;
    addr_t address;
    int xPosition;
    int yPosition;
    int goldCaptured;
} player_t;

typedef struct game {
    char* map;
    char* mapWithNoPlayers;
    int port;
    int mapHeight;
    int mapWidth;
    int encodedMapLength;
    hashtable_t* players;
    hashtable_t* goldPileAmounts;
    addr_t activePlayers[MaxPlayers]; // 26 max players
    int activePlayersCount;
    bool hasSpectator;
    addr_t spectatorAddress;
    int seed;
    char nextAvailableLetter;
    int goldRemaining;
    int minGoldPiles;
    int maxGoldPiles;
    bool plain;
} game_t;

/**************** functions ****************/

/**************** game_init ****************/
/* Initializes a new game with the given parameters.
 *
 * Caller provides:
 *   - mapFile: a file pointer to the map file to use for the game.
 *   - seed: an integer seed for random number generation.
 *   - gold: the total amount of gold in the game.
 *   - minGoldPiles: the minimum number of gold piles.
 *   - maxGoldPiles: the maximum number of gold piles.
 *   - plain: a boolean indicating whether the game is in plain mode.
 * We initialize:
 *   - The game structure with map data, players, gold piles, and other metadata.
 * Returns:
 *   - A pointer to the initialized game object or NULL on failure.
 */
game_t* game_init(FILE* mapFile, int seed, int gold, int minGoldPiles, int maxGoldPiles, bool plain);

/**************** game_playerMove ****************/
/* Handles movement for a player and updates their position and visible map.
 *
 * Caller provides:
 *   - playerAddress: the address of the player attempting to move.
 *   - game: a pointer to the current game state.
 *   - moveType: a character representing the direction of the move (e.g., 'h' for left).
 * We update:
 *   - The player's position and visible map.
 * Returns:
 *   - true if the move is valid and successful, false otherwise.
 */
bool game_playerMove(addr_t playerAddress, game_t* game, char moveType);

/**************** game_print ****************/
/* Prints the details of the given game object for debugging purposes.
 *
 * Caller provides:
 *   - game: a pointer to the game object to print.
 * We print:
 *   - Metadata about the game, including map dimensions, gold remaining, and the encoded map.
 */
void game_print(const game_t* game);

/**************** game_delete ****************/
/* Frees all resources associated with the given game object.
 *
 * Caller provides:
 *   - game: a pointer to the game object to delete.
 * We free:
 *   - Memory allocated for the map, players, gold piles, and other game-related resources.
 */
void game_delete(game_t* game);

/**************** game_playerInit ****************/
/* Initializes a new player and adds them to the game.
 *
 * Caller provides:
 *   - game: a pointer to the current game state.
 *   - address: the player's address for communication.
 *   - playerName: the name of the player as a string.
 * We initialize:
 *   - Player data, including name, position, map visibility, and assigned letter.
 * Returns:
 *   - A pointer to the newly created player or NULL if initialization fails.
 */
player_t* game_playerInit(game_t* game, addr_t address, char* playerName);

/**************** game_getFinalScores ****************/
/* Generates a string containing the final scores of all players in the game.
 *
 * Caller provides:
 *   - game: a pointer to the current game state.
 * We generate:
 *   - A string listing each player's letter, total gold captured, and name.
 * Returns:
 *   - A dynamically allocated string with the final scores. Caller is responsible for freeing it.
 */
char* game_getFinalScores(game_t* game);

#endif // __GAME_H
