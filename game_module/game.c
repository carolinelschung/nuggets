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
#include "../libcs50/file.h"


#define GoldTotal 250
#define GoldMinNumPiles 10
#define GoldMaxNumPiles 30
#define MaxPlayers 26
#define MAX_LINE_LENGTH 1024  // Set a reasonable max line length based on map constraints

/**************** local functions ****************/
char* encodeMap(FILE* mapFile, game_t* game);
void placeGold(game_t* game);
int getIndex(int x, int y, int mapWidth);
bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY); 
void printMap(char* map, game_t* game);
static void print_item(FILE* fp, const char* key, void* item);

game_t* game_init(FILE* mapFile, int seed)
{
    game_t* game = mem_malloc(sizeof(game_t));

    // Setup srand logic
    if (seed != 0) {
        srand(seed);
    } else {
        srand(getpid());
    }

    game->seed = seed;

    game->map = encodeMap(mapFile, game);
    game->goldRemaining = GoldTotal;
    game->players = hashtable_new(27);


    // initialize activePlayers array
    for (int i = 0; i < MaxPlayers; i++) {
        game->activePlayers[i] = NULL; // Zero-initialize each `addr_t` element
    }

    // Initialize player letters
    for (int i = 0; i < 26; i++) {
        game->playerLetters[i] = 'A' + i;
    }


    game->mapWithNoPlayers = mem_malloc((1+strlen(game->map))*sizeof(char));
    strcpy(game->mapWithNoPlayers, game->map);

    placeGold(game);

  
    return game;  // Return the initialized game struct
}


bool game_playerMove(addr_t playerAddress, game_t* game, char moveType)
{
    player_t* player = hashtable_find(game->players, message_stringAddr(playerAddress));
    if (player == NULL) return false; // Check for player existence to avoid NULL dereference

    int x, y;
    bool success;
    
    // Determine the proposed movement based on the moveType
    switch (moveType) {
        case 'h': // Move left
            x = player->xPosition - 1;
            y = player->yPosition;
            break;
        case 'l': // Move right
            x = player->xPosition + 1;
            y = player->yPosition;
            break;
        case 'j': // Move down
            x = player->xPosition;
            y = player->yPosition - 1;
            break;
        case 'k': // Move up
            x = player->xPosition;
            y = player->yPosition + 1;
            break;
        case 'y': // Move diagonal up and left
            x = player->xPosition - 1;
            y = player->yPosition + 1;
            break;
        case 'u': // Move diagonal up and right
            x = player->xPosition + 1;
            y = player->yPosition + 1;
            break;
        case 'b': // Move diagonal down and left
            x = player->xPosition - 1;
            y = player->yPosition - 1;
            break;
        case 'n': // Move diagonal down and right
            x = player->xPosition + 1;
            y = player->yPosition - 1;
            break;
        default:
            // Invalid move type, return without moving
            return false;
    }

    // Attempt to validate and move the player
    return success = validateAndMove(game, player, x, y);
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

    for (int i = 0; i < game->encodedMapLength; i++) {
        putchar(game->map[i]);
        // If we've reached the end of a row, print a newline
        if ((i + 1) % game->mapWidth == 0) {
            putchar('\n');
        }
    }
}



/**************** game_test ****************/
/* test scenario for game and map */
void game_test(game_t* game)
{
    
    int x, y;
    int seed = 21;  // Example seed value

    // Pass addresses of x, y, and seed to allow modification in map_player_init
    map_player_init(game->map, &x, &y, &seed, game->mapWidth, game->mapHeight);

    printf("Player initialized at position: (%d, %d)\n", x, y);

    char* visibleMap = mem_malloc((1+strlen(game->map)) * sizeof(char));

    visibleMap[y*game->mapWidth+x] = '@';

    map_get_visible(x, y, game->map, visibleMap, game->mapWidth, game->mapHeight);
    char* mapToPrint = map_decode(visibleMap, game);
    printf("%s", mapToPrint);

    visibleMap[y*game->mapWidth+x] = game->mapWithNoPlayers[y*game->mapWidth+x];

    x = x - 1;
    y = y + 1;
    visibleMap[y*game->mapWidth+x] = '@';

    char* previousMap = mem_malloc((1+strlen(visibleMap))*sizeof(char));
    strcpy(previousMap, visibleMap);

    map_get_visible(x, y, game->map, visibleMap, game->mapWidth, game->mapHeight);
    map_merge(visibleMap, previousMap);
    mapToPrint = map_decode(visibleMap, game);
    printf("%s", mapToPrint);

    visibleMap[y*game->mapWidth+x] = game->mapWithNoPlayers[y*game->mapWidth+x];

    x = x - 1;
    y = y + 1;
    visibleMap[y*game->mapWidth+x] = '@';

    strcpy(previousMap, visibleMap);

    map_get_visible(x, y, game->map, visibleMap, game->mapWidth, game->mapHeight);
    map_merge(visibleMap, previousMap);
    mapToPrint = map_decode(visibleMap, game);
    printf("%s", mapToPrint);

    visibleMap[y*game->mapWidth+x] = game->mapWithNoPlayers[y*game->mapWidth+x];

    x = x + 20;
    visibleMap[y*game->mapWidth+x] = '@';

    strcpy(previousMap, visibleMap);

    map_get_visible(x, y, game->map, visibleMap, game->mapWidth, game->mapHeight);
    map_merge(visibleMap, previousMap);
    mapToPrint = map_decode(visibleMap, game);
    printf("%s", mapToPrint);

}

void game_delete(game_t* game) {
    if (game == NULL) return;

    // Free map memory
    if (game->map) {
        mem_free(game->map);
    }
    if (game->mapWithNoPlayers) {
        mem_free(game->mapWithNoPlayers);
    }

    // Free gold piles
    if (game->goldPileAmounts) {
        hashtable_delete(game->goldPileAmounts, mem_free); // Ensures each entry is freed
    }

    // Free players hashtable
    if (game->players) {
        hashtable_delete(game->players, mem_free); // Pass mem_free if players have allocated memory
    }

    mem_free(game);
}

player_t* game_playerInit(game_t* game, addr_t address, char* playerName)
{
    player_t* player = mem_malloc(sizeof(player_t));
    for (int i = 0; i < 26; i++) {
        if (game->activePlayers[i] == NULL) {
            game->activePlayers[i] = &address;
            player->address = address;
            player->playerLetter = game->playerLetters[i];
            player->playerName = playerName;
            game->activePlayersCount+=1;
            hashtable_insert(game->players, message_stringAddr(address), player);

            int x, y;
            player->playerMap = map_player_init(game->map, &x, &y, &(game->seed), game->mapWidth, game->mapHeight);
            player->xPosition = x;
            player->yPosition = y;
            return player;
            
        }
    }
    return NULL;
}


/************* HELPER FUNCTIONS *****************/

/**************** encodeMap ****************/
/* Reads the map file line by line using file_readLine and sets the map dimensions */
char* encodeMap(FILE* mapFile, game_t* game) 
{
    if (mapFile == NULL) {
        fprintf(stderr, "Error: map file cannot be null.\n");
        return NULL;
    }

    // Initialize map data and properties
    size_t mapSize = 0;
    size_t bufferSize = 1024;
    char* map = mem_malloc(bufferSize);
    if (map == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(mapFile);
        return NULL;
    }

    game->mapWidth = 0;
    game->mapHeight = 0;

    // Read each line from the file and append to map buffer
    char* line;
    while ((line = file_readLine(mapFile)) != NULL) {
        size_t lineLen = strlen(line);

        // Set map width based on the first line
        if (game->mapWidth == 0) {
            game->mapWidth = lineLen;
        } else if (lineLen != game->mapWidth) {
            fprintf(stderr, "Error: Inconsistent line length in map file.\n");
            free(line);
            free(map);
            fclose(mapFile);
            return NULL;
        }

        // Ensure map buffer has enough space for the new line
        if (mapSize + lineLen + 1 >= bufferSize) {
            bufferSize *= 2;
            char* newMap = realloc(map, bufferSize);
            if (newMap == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed.\n");
                free(line);
                free(map);
                fclose(mapFile);
                return NULL;
            }
            map = newMap;
        }

        // Append line to map and update map size
        memcpy(map + mapSize, line, lineLen);
        mapSize += lineLen;
        free(line);  // Free line after it’s copied to the map buffer
        game->mapHeight++;
    }

    map[mapSize] = '\0';  // Null-terminate the map
    printf("width: %d, height %d\n", game->mapWidth, game->mapHeight);
    game->encodedMapLength = game->mapWidth * game->mapHeight;

    fclose(mapFile);
    return map;  // Return the map buffer
}




/**************** placeGold ****************/
/* Places gold randomly on map for a given game
 * Init and populate goldPileAmount Hashtable
 */
void placeGold(game_t* game) {
    if (game == NULL || game->map == NULL || game->encodedMapLength == 0) {
        fprintf(stderr, "Error: Game or map is not initialized, or map length is zero.\n");
        return;
    }

    int numPiles = GoldMinNumPiles + rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1);
    int goldRemaining = GoldTotal;
    game->goldPileAmounts = hashtable_new(numPiles);

    for (int i = 0; i < numPiles; i++) {
        // Generate a pile size with a bias toward the middle of the range 5-20
        int pileSize = (rand() % 16 + 5 + rand() % 16 + 5) / 2;  // Average of two random numbers in the range 5-20
        if (pileSize > goldRemaining) pileSize = goldRemaining;   // Avoid overspending
        goldRemaining -= pileSize;

        // Allocate memory for pileSize to ensure each entry is unique
        int* pileSizePtr = malloc(sizeof(int));
        if (pileSizePtr == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for pileSize.\n");
            return;
        }
        *pileSizePtr = pileSize;

        bool spotFound = false;
        char key[12];
        while (!spotFound) {
            int randIndex = rand() % game->encodedMapLength;
            if (game->map[randIndex] == '.') {
                game->map[randIndex] = '*';
                snprintf(key, sizeof(key), "%d", randIndex);
                hashtable_insert(game->goldPileAmounts, key, pileSizePtr);
                spotFound = true;
                printf("Placed %d gold at position %d\n", *pileSizePtr, randIndex);
            }
        }
    }

    hashtable_print(game->goldPileAmounts, stdout, print_item);
}



bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY) 
{
    int currentIndex = player->yPosition * game->mapWidth + player->xPosition;
    char currentTilePlayerIsOn = game->mapWithNoPlayers[currentIndex];
    int proposedIndex = proposedY * game->mapWidth + proposedX;
    char proposedTile = game->map[proposedIndex];

    if (proposedTile != '.' && proposedTile != '#' && proposedTile != '*') {
        return false;
    }

    game->map[currentIndex] = currentTilePlayerIsOn;

    if (game->map[proposedIndex] == '*') {
        // Convert proposedIndex to a string for lookup in the hashtable
        char key[12];
        snprintf(key, sizeof(key), "%d", proposedIndex);

        int* goldAmountPtr = hashtable_find(game->goldPileAmounts, key);
        int goldAmountPlayerFound = *goldAmountPtr;

        player->goldCaptured += goldAmountPlayerFound;
        game->goldRemaining -= goldAmountPlayerFound;
        game->map[proposedIndex] = '.';

        // Free the gold amount after retrieval
        if (goldAmountPtr) {
            mem_free(goldAmountPtr);
            hashtable_insert(game->goldPileAmounts, key, NULL);
        }
    }

    game->map[proposedIndex] = player->playerLetter;

    player->xPosition = proposedX;
    player->yPosition = proposedY;
    
    // It fails after printing this line
    printf("x %d, y %d\n", proposedX, proposedY);
    
    char* visibleMap = mem_malloc(sizeof(char) * strlen(game->map));
    map_get_visible(player->xPosition, player->yPosition, game->map, visibleMap, game->mapWidth, game->mapHeight);
    map_merge(player->playerMap, visibleMap);
    
    return true;
}

/**************** game_printMap ****************/
/* prints map of a game for testing */
void printMap(char* map, game_t* game)
{
    for (int i = 0; i < game->encodedMapLength; i++) {
        putchar(map[i]);
        // If we've reached the end of a row, print a newline
        if ((i + 1) % game->mapWidth == 0) {
            putchar('\n');
        }
    }    
}


static void print_item(FILE* fp, const char* key, void* item) {
    int* amt = item;
    if (amt == NULL) {
        printf("(null)");
    } else {
        printf("Key: %s, Item: %d", key, *amt);
    }

}