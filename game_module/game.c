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
#define MAX_NAME_LENGTH 50

/**************** local functions ****************/
char* encodeMap(FILE* mapFile, game_t* game);
void placeGold(game_t* game);
int getIndex(int x, int y, int mapWidth);
bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY); 
void printMap(char* map, game_t* game);
static void print_item(FILE* fp, const char* key, void* item);
player_t* getPlayerByLetter(char letter, game_t* game);
void getPlayerByLetterHelper(void* arg, const char* key, void* item);
static void player_delete(void* playerRaw);

game_t* game_init(FILE* mapFile, int seed, int gold, int minGoldPiles, int maxGoldPiles, bool plain)
{
    game_t* game = mem_malloc(sizeof(game_t));

    // Setup srand logic
    if (seed != 0) {
        srand(seed);
    } else {
        srand(getpid());
    }

    game->seed = seed;
    game->goldRemaining = gold;
    game->minGoldPiles = minGoldPiles;
    game->maxGoldPiles = maxGoldPiles;
    game->plain = plain;


    game->map = encodeMap(mapFile, game);
    game->players = hashtable_new(27);

    game->nextAvailableLetter = 'A';


    // // initialize activePlayers array
    // for (int i = 0; i < MaxPlayers; i++) {
    //     game->activePlayers[i] = NULL; // Zero-initialize each `addr_t` element
    // }

    // Initialize player letters
    for (int i = 0; i < 26; i++) {
        game->playerLetters[i] = 'A' + i;
    }


    game->mapWithNoPlayers = mem_malloc((1+strlen(game->map))*sizeof(char));
    strcpy(game->mapWithNoPlayers, game->map);

    game->activePlayersCount = 0;

    placeGold(game);

    printMap(game->map, game);

  
    return game;  // Return the initialized game struct
}


bool game_playerMove(addr_t playerAddress, game_t* game, char moveType)
{
    player_t* player = hashtable_find(game->players, message_stringAddr(playerAddress));
    if (player == NULL) return false; // Check for player existence to avoid NULL dereference

    int x = player->xPosition;
    int y = player->yPosition;

    // Determine the proposed movement based on the moveType
    switch (moveType) {
        case 'h': // Move left
            return validateAndMove(game, player, x - 1, y);
        case 'l': // Move right
            return validateAndMove(game, player, x + 1, y);
        case 'j': // Move down
            return validateAndMove(game, player, x, y + 1);
        case 'k': // Move up
            return validateAndMove(game, player, x, y - 1);

        // Diagonal moves for lowercase letters
        case 'y': // Move diagonal up and left
            return validateAndMove(game, player, x - 1, y - 1);
        case 'u': // Move diagonal up and right
            return validateAndMove(game, player, x + 1, y - 1);
        case 'b': // Move diagonal down and left
            return validateAndMove(game, player, x - 1, y + 1);
        case 'n': // Move diagonal down and right
            return validateAndMove(game, player, x + 1, y + 1);

        // Capital letters for maximum moves in each direction
        case 'H': // Move maximum left
            while (validateAndMove(game, player, x - 1, y)) {
                x--;  // Update x to the next position left
            }
            break;
        case 'L': // Move maximum right
            while (validateAndMove(game, player, x + 1, y)) {
                x++;  // Update x to the next position right
            }
            break;
        case 'K': // Move maximum up
            while (validateAndMove(game, player, x, y - 1)) {
                y--;  // Update y to the next position up
            }
            break;
        case 'J': // Move maximum down
            while (validateAndMove(game, player, x, y + 1)) {
                y++;  // Update y to the next position down
            }
            break;

        // Capital letters for diagonal maximum moves
        case 'Y': // Move maximum diagonal up and left
            while (validateAndMove(game, player, x - 1, y - 1)) {
                x--;
                y--;
            }
            break;
        case 'U': // Move maximum diagonal up and right
            while (validateAndMove(game, player, x + 1, y - 1)) {
                x++;
                y--;
            }
            break;
        case 'B': // Move maximum diagonal down and left
            while (validateAndMove(game, player, x - 1, y + 1)) {
                x--;
                y++;
            }
            break;
        case 'N': // Move maximum diagonal down and right
            while (validateAndMove(game, player, x + 1, y + 1)) {
                x++;
                y++;
            }
            break;
        default:
            return false;
    }

    // Update player's final position after completing the loop
    player->xPosition = x;
    player->yPosition = y;
    return true;
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


void game_delete(game_t* game) {
    if (game == NULL) return;

    // Free map memory
    //if (game->map) {
        mem_free(game->map);
    //}
    //if (game->mapWithNoPlayers) {
        mem_free(game->mapWithNoPlayers);
    //}
 

    // Free gold piles
    //if (game->goldPileAmounts) {
        hashtable_delete(game->goldPileAmounts, mem_free); // Ensures each entry is freed
    //}

    // Free players hashtable
    //if (game->players) {
        hashtable_delete(game->players, player_delete); // Pass mem_free if players have allocated memory
    //}

    mem_free(game);
}


static void player_delete(void* playerRaw){
    player_t* player = (player_t*) playerRaw;
    mem_free(player->playerMap);
    mem_free(player);
}

player_t* game_playerInit(game_t* game, addr_t address, char* playerName)
{
    player_t* player = mem_malloc(sizeof(player_t));
    if (player == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for player.\n");
        return NULL;
    }

    // Check if we have available letters left
    if (game->nextAvailableLetter > 'Z') {
        fprintf(stderr, "Error: Maximum number of players reached.\n");
        mem_free(player);
        return NULL;
    }

    // Find the first available slot in activePlayers for a new player
    for (int i = 0; i < MaxPlayers; i++) {
        if (!message_isAddr(game->activePlayers[i])) {  // Check if the slot is available
            // Assign address to this slot and set the player's letter
            game->activePlayers[i] = address;
            player->address = address;
            player->playerLetter = game->nextAvailableLetter;  // Assign the current available letter
            game->activePlayersCount++;

            // Prepare for the next player by moving to the next letter
            game->nextAvailableLetter++;

            // Allocate memory for playerName and copy it safely with null termination
            player->playerName = mem_malloc(MAX_NAME_LENGTH + 1);
            if (player->playerName != NULL) {
                strncpy(player->playerName, playerName, MAX_NAME_LENGTH);
                player->playerName[MAX_NAME_LENGTH] = '\0';  // Ensure null termination
            } else {
                fprintf(stderr, "Error: Failed to allocate memory for playerName.\n");
                mem_free(player);  // Free player memory if name allocation fails
                return NULL;
            }

            // Insert player into the hashtable using the address as the key
            hashtable_insert(game->players, message_stringAddr(address), player);

            printf("New player initialized with name: %s, letter: %c\n", player->playerName, player->playerLetter);
            fflush(stdout);

            // Initialize player's position using map_player_init
            int x, y;
            map_player_init(game->map, &x, &y, &(game->seed), game->mapWidth, game->mapHeight, game);
            player->xPosition = x;
            player->yPosition = y;

            // Allocate and initialize player map
            player->playerMap = mem_malloc(sizeof(char) * (strlen(game->map) + 1));
            if (player->playerMap == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for playerMap.\n");
                mem_free(player->playerName);
                mem_free(player);
                return NULL;
            }

            map_get_visible(x, y, game->map, player->playerMap, game->mapWidth, game->mapHeight);

            // Add player’s letter to the map and player's map
            int index = y * game->mapWidth + x;
            game->map[index] = player->playerLetter;
            player->playerMap[index] = '@';

            return player;  // Successfully initialized player
        }
    }

    // If no available slot is found, free allocated memory and return NULL
    mem_free(player);
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
            mem_free(line);
            mem_free(map);
            fclose(mapFile);
            return NULL;
        }

        // Ensure map buffer has enough space for the new line
        if (mapSize + lineLen + 1 >= bufferSize) {
            bufferSize *= 2;
            char* newMap = realloc(map, bufferSize);
            if (newMap == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed.\n");
                mem_free(line);
                mem_free(map);
                fclose(mapFile);
                return NULL;
            }
            map = newMap;
        }

        // Append line to map and update map size
        memcpy(map + mapSize, line, lineLen);
        mapSize += lineLen;
        mem_free(line);  // Free line after it’s copied to the map buffer
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

    int numPiles = game->minGoldPiles + rand() % (game->maxGoldPiles - game->minGoldPiles + 1);
    int remainingGold = game->goldRemaining - numPiles;

    game->goldPileAmounts = hashtable_new(numPiles);

    int pileValues[numPiles];
    for (int i = 0; i < numPiles; i++) {
        pileValues[i] = 1;
    }

    // Distribute the remaining gold among the piles
    while (remainingGold > 0) {
        int randomPile = rand() % numPiles;
        pileValues[randomPile]++;
        remainingGold--;
    }

    for (int i = 0; i < numPiles; i++) {
        int* pileSizePtr = malloc(sizeof(int));
        *pileSizePtr = pileValues[i];
        bool spotFound = false;
        while (!spotFound) {
            int randIndex = rand() % game->encodedMapLength;
            if (game->map[randIndex] == '.') {
                game->map[randIndex] = '*';
                char key[12];
                snprintf(key, sizeof(key), "%d", randIndex);
                hashtable_insert(game->goldPileAmounts, key, pileSizePtr);
                spotFound = true;
                printf("Placed %d gold at position %d\n", *pileSizePtr, randIndex);
            }
        }
    }

    hashtable_print(game->goldPileAmounts, stdout, print_item);
}


char* game_getFinalScores(game_t* game)
{   
    // Allocate memory for the final scores message
    char* quitMessage = mem_malloc(message_MaxBytes * sizeof(char));
    if (quitMessage == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for quitMessage.\n");
        return NULL;
    }

    // Iterate over letters 'A' to 'Z' and build the message
    for (char letter = 'A'; letter <= 'Z'; letter++) {
        // Find player with the current letter
        player_t* player = getPlayerByLetter(letter, game);
        if (player != NULL) {
            // Format the player's information and append to quitMessage
            char playerInfo[100];
            snprintf(playerInfo, sizeof(playerInfo), "%-2c %10d %-51s\n", 
                     player->playerLetter, player->goldCaptured, player->playerName);
            strncat(quitMessage, playerInfo, message_MaxBytes - strlen(quitMessage) - 1);
        }
    }

    return quitMessage;  // Return the accumulated message
}

// Function to find a player by letter in the hashtable
player_t* getPlayerByLetter(char letter, game_t* game)
{   
    player_t* foundPlayer = NULL;

    // Use an array to hold the letter and pointer to foundPlayer
    void* arg[2];
    arg[0] = &letter;
    arg[1] = &foundPlayer;

    // Iterate through the hashtable to find the player with the matching letter
    hashtable_iterate(game->players, arg, getPlayerByLetterHelper);

    // Return the found player or NULL if not found
    return foundPlayer;
}

// Helper function for hashtable iteration to find a player by letter
void getPlayerByLetterHelper(void* arg, const char* key, void* item)
{   
    // Retrieve the letter and pointer to foundPlayer from the arg array
    char targetLetter = *(char*)(((void**)arg)[0]);
    player_t** foundPlayerPtr = (player_t**)(((void**)arg)[1]);

    player_t* player = (player_t*)item;

    // Check if the player's letter matches the target letter
    if (player->playerLetter == targetLetter) {
        *foundPlayerPtr = player;  // Set the found player via the pointer
    }
}


//printf("%-2c %10d %-10s\n", 'A' + i, scores[i], players[i]);


bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY) 
{
    int currentIndex = player->yPosition * game->mapWidth + player->xPosition;
    char currentTilePlayerIsOn = game->mapWithNoPlayers[currentIndex];
    int proposedIndex = proposedY * game->mapWidth + proposedX;
    char proposedTile = game->map[proposedIndex];

    char valid_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (proposedTile != '.' && proposedTile != '#' && proposedTile != '*' && strchr(valid_chars, proposedTile) == false) {
        return false;
    }

    if (strchr(valid_chars, proposedTile) && game->plain) {
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
        player->goldJustCaptured = goldAmountPlayerFound;
        game->map[proposedIndex] = '.';

        // Free the gold amount after retrieval
        if (goldAmountPtr) {
            //mem_free(goldAmountPtr);
            hashtable_insert(game->goldPileAmounts, key, NULL);
        }
    }

    if (strchr(valid_chars, proposedTile) && !game->plain) {
        int index = proposedTile - 'A';  // Calculate the index in activePlayers based on the letter
        const char* stringAddress = message_stringAddr(game->activePlayers[index]);
        player_t* playerMovedOnto = hashtable_find(game->players, stringAddress);

        if (playerMovedOnto != NULL) {
            printf("Player %c moved onto player %c\n", player->playerLetter, playerMovedOnto->playerLetter);

            // Steal gold from the player being moved onto
            player->goldCaptured += playerMovedOnto->goldCaptured;
            playerMovedOnto->goldCaptured = 0;

            // Swap positions
            int tempX = playerMovedOnto->xPosition;
            int tempY = playerMovedOnto->yPosition;
            playerMovedOnto->xPosition = player->xPosition;
            playerMovedOnto->yPosition = player->yPosition;
            player->xPosition = tempX;
            player->yPosition = tempY;

            // Update map to reflect swapped positions
            game->map[playerMovedOnto->yPosition * game->mapWidth + playerMovedOnto->xPosition] = playerMovedOnto->playerLetter;
            game->map[player->yPosition * game->mapWidth + player->xPosition] = player->playerLetter;
        } else {
            printf("Error: Could not find player at activePlayers[%d]\n", index);
            return false;
        }
    }

    
    game->map[proposedIndex] = player->playerLetter;

    player->xPosition = proposedX;
    player->yPosition = proposedY;
    
    // It fails after printing this line
    printf("x %d, y %d\n", proposedX, proposedY);
    
    char* visibleMap = mem_malloc(sizeof(char) * (strlen(game->map) + 1));
    map_get_visible(player->xPosition, player->yPosition, game->map, visibleMap, game->mapWidth, game->mapHeight);
    map_merge(player->playerMap, visibleMap);

    mem_free(visibleMap);

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

