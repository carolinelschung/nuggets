/* 
 * game.c - CS50 Nuggets game module
 *
 * see game.h for more information.
 *
 * Joseph Quaratiello, November 2024
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/mem.h"
#include "game.h"

#define GoldTotal 250
#define GoldMinNumPiles 10
#define GoldMaxNumPiles 30
#define MaxPlayers 26
#define MAX_LINE_LENGTH 1024  // Set a reasonable max line length based on map constraints

/**************** local types ****************/
typedef struct player {
    char* playerName;
    char* playerMap;
    char* address;
    char role;
    int xPosition;
    int yPosition;
    int goldCaptured;
    bool isActive;
} player_t;

/**************** local functions ****************/
char* encodeMap(FILE* mapFile, game_t* game);

game_t* game_init(FILE* mapFile, int seed)
{
    game_t* game = mem_malloc_assert(sizeof(game_t), "game struct allocation");

    game->map = encodeMap(mapFile, game);
    game->goldRemaining = GoldTotal;
    game->players = hashtable_new(27);
    
    // initialize activePlayers array
    for (int i = 0; i < MaxPlayers; i++) {
        game->activePlayers[i] = NULL;
    }

    return game;  // Return the initialized game struct
}

/**************** loadMap ****************/
/* Reads the map file and returns it as a string, setting the map length and width. */
char* encodeMap(FILE* mapFile, game_t* game) 
{
    if (mapFile == NULL) {
        fprintf(stderr, "Error: map file cannot be null.\n");
        return NULL;
    }

    // Initialize map string buffer
    char* map = mem_malloc(1); // Start with an empty string
    size_t mapSize = 0;        // Total size of the map string (for allocation)
    game->mapHeight = 0;       // Initialize map height
    game->mapWidth = 0;        // Initialize map width (set by the first line)

    char line[MAX_LINE_LENGTH];

    // Read the map file line by line
    while (fgets(line, MAX_LINE_LENGTH, mapFile) != NULL) {
        size_t lineLen = strlen(line);

        // Remove newline character at the end of the line, if present
        if (line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';
            lineLen--;
        }

        // Set mapWidth based on the first line (excluding newline character)
        if (game->mapWidth == 0) {
            game->mapWidth = lineLen;  // Set width for all lines; excludes newline
        }

        // Allocate enough memory for the existing map and new line
        // Replace mem_realloc with realloc in loadMap function
        char* newMap = realloc(map, mapSize + lineLen + 1);

        if (newMap == NULL) {
            fprintf(stderr, "Error: Memory allocation failed while loading map.\n");
            free(map);
            fclose(mapFile);
            return NULL;
        }
        map = newMap;

        // Append the current line to the map string
        memcpy(map + mapSize, line, lineLen);
        mapSize += lineLen;
        game->mapHeight++;  // Increment height for each line read
    }

    // Null-terminate the map string
    map[mapSize] = '\0';

    // Calculate the map's encoded length based on width and height
    game->encodedMapLength = game->mapWidth * game->mapHeight;

    fclose(mapFile);
    return map;
}

void game_print(const game_t* game)
{
    if (game == NULL) {
        printf("Game is not initialized.\n");
        return;
    }

    printf("Game initialized with the following details:\n");
    printf("Map Dimensions: %dx%d\n", game->mapHeight, game->mapWidth);
    printf("Encoded Map Length: %d\n", game->encodedMapLength);
    printf("Gold Remaining: %d\n", game->goldRemaining);

    printf("Encoded Map:\n%s\n", game->map);
}

