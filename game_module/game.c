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
#include <unistd.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/mem.h"
#include "game.h"
#include "../support/message.h"
#include "../map_module/map.h"

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
void placeGold(game_t* game);
int getIndex(int x, int y, int mapWidth);
bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY); 
static void delete_item(void* item);

game_t* game_init(FILE* mapFile, int seed)
{
    game_t* game = mem_malloc_assert(sizeof(game_t), "game struct allocation");

    // Setup srand logic
    if (seed != 0) {
        srand(seed);
    } else {
        srand(getpid());
    }

    game->map = encodeMap(mapFile, game);
    game->mapWithNoPlayers = encodeMap(mapFile, game);
    game->goldRemaining = GoldTotal;
    game->players = hashtable_new(27);

    // initialize activePlayers array
    for (int i = 0; i < MaxPlayers; i++) {
        game->activePlayers[i] = NULL;
    }

    placeGold(game);

    game_print(game);
    
    return game;  // Return the initialized game struct
}


void game_playerMove(addr_t playerAddress, game_t* game, char moveType)
{

    player_t* player = hashtable_find(game->players, message_stringAddr(playerAddress));
    char* map = game->map;
    int mapWidth = game->mapWidth;
    
    switch (moveType) {
        case 'h':
            //move left
            int x = player->xPosition - 1;
            int y = player->yPosition;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            
            break;
        case 'l':
            //move right
            int x = player->xPosition + 1;
            int y = player->yPosition;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
        case 'j':
            //move down
            int x = player->xPosition;
            int y = player->yPosition - 1;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
        case 'k':
            //move up
            int x = player->xPosition;
            int y = player->yPosition + 1;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
        case 'y':
            //move diagonal up and left
            int x = player->xPosition - 1;
            int y = player->yPosition + 1;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
        case 'u':
            //move diagonal up and right
            int x = player->xPosition + 1;
            int y = player->yPosition + 1;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
        case 'b':
            //move diagonal down and left
            int x = player->xPosition - 1;
            int y = player->yPosition - 1;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
        case 'n':
            //move diagonal down and right
            int x = player->xPosition + 1;
            int y = player->yPosition - 1;
            bool success = validateAndMove(game->map, game->mapWithNoPlayers, x, y); 
            break;
    }
}

/**************** game_print ****************/
/* Prints metadata of a game object */
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
    
    int goldExample = hashtable_find(game->goldPileAmounts, 10);

    for (int i = 0; i < game->encodedMapLength; i++) {
        putchar(game->map[i]);
        // If we've reached the end of a row, print a newline
        if ((i + 1) % game->mapWidth == 0) {
            putchar('\n');
        }
    }
}


/************* HELPER FUNCTIONS *****************/

/**************** encodeMap ****************/
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


/**************** placeGold ****************/
/* Places gold randomly on map for a given game
 * Init and populate goldPileAmount Hashtable
 */
void placeGold(game_t* game)
{
    if (game == NULL || game->map == NULL) {
        fprintf(stderr, "Game or map is not initialized.\n");
        return;
    }


    // Get random number of piles between GoldMinNumPiles and GoldMaxNumPiles
    int numPiles = GoldMinNumPiles + rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1);
    int goldRemaining = GoldTotal;

    game->goldPileAmounts = hashtable_new(numPiles); // initialize goldPileAmounts to have numPiles slots

    for (int i = 0; i < numPiles; i++) 
    {
        int maxPileSize = goldRemaining - (numPiles - i - 1); //Makes sure enough gold
        int pileSize = 1 + rand() % maxPileSize;

        goldRemaining -= pileSize;

        bool spotFound = false;
        while (!spotFound) {
            int randIndex = rand() % game->encodedMapLength;
            if (game->map[randIndex] == '.') {
                game->map[randIndex] = '*';
                hashtable_insert(game->goldPileAmounts, randIndex, pileSize);
                spotFound = true;
            }
        }
    }
}

int getIndex(int x, int y, int mapWidth)
{
    return y * mapWidth + x;
}

bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY) 
{

    int currentIndex = player->yPosition * game->mapWidth + player->xPosition;
    char currentTilePlayerIsOn = game->mapWithNoPlayers[currentIndex];
    int proposedIndex = proposedY * game->mapWidth + proposedX;
    char proposedTile = game->map[proposedIndex];

    if (proposedTile != '.' || proposedTile != '#' || proposedTile != '*')
    {
        return false;
    }

    // BEGIN MAKING THE PLAYER MOVE

    //TODO: Add visibility implemenation

    game->map[currentIndex] = currentTilePlayerIsOn; // sets the tile to be what is "under" player
    
    if (game->map[proposedIndex] == '*') {
        int goldAmountPlayerFound = hashtable_find(game->goldPileAmounts, proposedIndex);
        if (goldAmountPlayerFound != NULL) {
            player->goldCaptured += goldAmountPlayerFound;
            game->goldRemaining -= goldAmountPlayerFound;
            game->map[proposedIndex] = '.';
        }
    }

    game->map[proposedIndex] = '@'; // change new tile to player character

    player->xPosition = proposedX;
    player->yPosition = proposedY;

    char* visibleMap = mem_malloc(sizeof(char)*strlen(game->map));
    map_get_visible(player->xPosition, player->yPosition, game->map, visibleMap, game->mapWidth, game->mapHeight);

    map_merge(player->playerMap, visibleMap);

    mem_free(visibleMap);

    return true;
}
