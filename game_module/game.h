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
    char playerLetters[26];
    char nextAvailableLetter;
    int goldRemaining;
    int minGoldPiles;
    int maxGoldPiles;
    bool plain;
} game_t;

/**************** functions ****************/

/**************** game_initialize ****************/
/* Initializes a new game
 *
 * Caller provides:
 *   - Encoded map file string
 *   - optional seed
 * Caller is responsible for:
 *   later calling game_cleanup
 */
game_t* game_init(FILE* mapFile, int seed, int gold, int minGoldPiles, int maxGoldPiles, bool plain);

/**************** game_updateState ****************/
/* Updates the state of the game based on a player's action.
 *
 * Caller provides:
 *   - player: a pointer to the player struct representing the player taking action
 *   - action: a character representing the action to be processed (e.g., movement or quit)
 * We update:
 *   - The game state, including player positions, collected gold, and map updates.
 * Caller is responsible for:
 *   - Calling game_cleanup when the game is over to free allocated resources.
 */
void game_updateState(player_t* player, char action);

/**************** playerMove ****************/
/* Updates the player's visible map based on their movement.
 *
 * Caller provides:
 *   - currentVisibleMap: a pointer to the player's current visible portion of the map
 *   - moveType: a character representing the type of move (e.g., 'h' for left, 'j' for down)
 *   - xPos: the player's current x-coordinate on the map
 *   - yPos: the player's current y-coordinate on the map
 */
bool game_playerMove(addr_t playerAddress, game_t* game, char moveType);


/**************** game_print ****************/
/* Prints the a object 
 *
 * Caller provides:
 *   - game object to print
 */
void game_print(const game_t* game);

void game_delete(game_t* game);

player_t* game_playerInit(game_t* game, addr_t address, char* playerName);

char* game_getFinalScores(game_t* game);



#endif // __GAME_H