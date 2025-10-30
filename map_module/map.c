// Source code of map module for the Nuggets project
// CS50, 24F
// Team 10, Anna Filyurina, Nov. 2024

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<math.h>
#include "../game_module/game.h"
#include "../libcs50/mem.h"


// LOCAL FUNCTIONS

/*
Helper function that determines the visibility of one point and returns true or false
*/
static bool isVisible(int x, int y, int ptX, int ptY, char* masterMap, const int NC);


/// GLOBAL FUNCTIONS 

/* *** map_player_init ***

Inputs:
char* mainMap - the always up to date map with all players and all gold passed from game module
int* x - empty initializaed pointer where the number of column the player is in will be stored
int* y - empty initializaed pointer where the number of row the player is in will be stored
int* seed - a seed for randomization optionally provided by the user
const int NC - number of columns in the map 
const int NR - number of rows in the map

Function that takes in seed and based on it returns a player's starting coordinates on the mainMap.

*/
void map_player_init(char* masterMap, int* x, int* y, int* seed, const int NC, const int NR, game_t* game){
  // The location of the character in the map character string
  int location = 0;
  
  // If seed is not null, use it to generate pseudo-random numbers 
  if(seed == NULL){
    srand(getpid());
  }else{
    printf("active player count: %d", game->activePlayersCount);
    srand(*seed + game->activePlayersCount);
  }

  // Geerate new random numbers untill the location on map is a valid empty room point
  while(masterMap[location] != '.'){
    
    *x = rand() % NC;
    *y = rand() % NR;
    location = (*y)*NC + (*x);
  }
}

/* *** map_get_visible ***

Inputs:
int x - a vaild player posistion (number of column the player is in)
int y - a vaild player posistion (number of row the player is in)
char* mainMap - the always up to date map with all players and all gold passed from game module
char* visibleMap - an empty initialized string that has enough memory allocated to store a map
const int NC - number of columns in the map 
const int NR - number of rows in the map

Function that takes in a player's coordinates and the main map and puts the visibleMap (only what the payer sees immediately) 
into a previously allocated string. 

*/
void map_get_visible(int x, int y, char* masterMap, char* visibleMap, const int NC, const int NR){
  int length = strlen(masterMap);
  int ptX, ptY; // coordinaets of the point we want to determine the visibility of 
  for(int i = 0; i < length; i++){
    ptY = i/NC;
    ptX = i - ptY*NC;
    if(isVisible(x, y, ptX, ptY, masterMap, NC)){
      visibleMap[i] = masterMap[i];
    }else{
      visibleMap[i] = ' ';
    }
  }

  // put the player on the map
  visibleMap[y*NC+x] = '@';
}

/* ** (local) isVisible ***

Inputs:
int x - player location (colunm)
int y - player location (row)
int ptX - locaiton of the point we want to determine the visibility of (column)
int ptY - locaiton of the point we want to determine the visibility of (row)
char* mainMap - the always up to date map with all players and all gold passed from game module
const int NC - number of columns in the map 

*/
static bool isVisible(int x, int y, int ptX, int ptY, char* masterMap, const int NC){
  float k; // k is the slope coefficient
  int yNew, xNew;
  int location;
  
  if(ptY == y){

    if(x - ptX < 0){
      for(int i = 1; i < abs(x - ptX); i++){
        yNew = y; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z'))){
          return false;
        }
      }
    }else{
      for(int i = -1; i > (-1)*(x - ptX); i--){
        yNew = y; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z'))){
          return false;
        }
      }
    }
    
    return true;

  }else if(ptX == x){
    
    if(y - ptY > 0){
      for(int i = -1; i > (-1)*(y - ptY); i--){
        yNew = y + i;
        xNew = x;
        location = (yNew)*NC + xNew;
        if(masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')){
          return false;
        }
      }
    }else{
      for(int i = 1; i < abs(y - ptY); i++){
        yNew = y + i; // the fractional part is discarded in this case
        xNew = x;
        location = (yNew)*NC + xNew;
        if(masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')){
          return false;
        }
      }
    }
    
    return true;

  }else{
    float deltaY, deltaX; // avoiding integer devision
    float intermediate;
    deltaY = ptY - y;
    deltaX = ptX - x;
    k = deltaY/deltaX;

    if((x-ptX) > 0){
      for(int i = -1; i > (-1)*(x - ptX); i--){
        intermediate = i*k;
        yNew = y + intermediate; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + NC] != '.' && masterMap[location + NC] != '*' && (masterMap[location+NC] < 'A' || masterMap[location+NC] > 'Z'))){
          return false;
        }
      }
    }else{
      for(int i = 1; i < abs(x - ptX); i++){
        intermediate = i*k;
        yNew = y + intermediate; // the fractional part is discarded in this case
        xNew = x + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + NC] != '.' && masterMap[location + NC] != '*' && (masterMap[location+NC] < 'A' || masterMap[location+NC] > 'Z'))){
          return false;
        }
      }
    }

    if((y - ptY) > 0){
      for(int i = -1; i > (-1)*(y - ptY); i--){
        intermediate = i*(1/k);
        xNew = x + intermediate; // (round this value down)
        yNew = y + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if ((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + 1] != '.' && masterMap[location + 1] != '*'  && (masterMap[location+1] < 'A' || masterMap[location+1] > 'Z'))){
          return false;
        }
      }
    }else{
      for(int i = 1; i < abs(y - ptY); i++){
        intermediate = i*(1/k);
        xNew = x + intermediate; // (round this value down)
        yNew = y + i;
        location = (yNew)*NC + xNew ;
        if(location < 0){
          printf("your math is wrong\n");
          return true;
        }
        if ((masterMap[location] != '.' && masterMap[location] != '*' && (masterMap[location] < 'A' || masterMap[location] > 'Z')) && (masterMap[location + 1] != '.' && masterMap[location + 1] != '*' && (masterMap[location+1] < 'A' || masterMap[location+1] > 'Z'))){
          return false;
        }
      }
    }
    
    return true;
  }
}

/* *** map_merge ***

char* playerMap - most relevant map of the player (what the user saw before the current update)
char* visibleMap - map of what the player sees immediately from the spot they are at (generated by map_get_visible)
int NC and int NR - dimentions of the map

Function that takes in the visible map and the payer's presious map and merges them 
omitting the gold and other players from previous map. Most relevant map of the player 
is in playerMap as a result of this function.

*/
void map_merge(char* playerMap, char* visibleMap, int NC, int NR){
  if(playerMap == NULL || visibleMap == NULL){
    return;
  }
  int length = NC*NR;
  for(int i = 0; i < length; i++){
    if(playerMap[i] == '*' || (playerMap[i] >= 'A' && playerMap[i] <= 'Z')){
      playerMap[i] = '.';
    }
    if(visibleMap[i] != ' '){
      playerMap[i] = visibleMap[i];
    }
  }
}

/* *** map_decode ***

Inputs:
game_t* game - the main game struct to access dimensions 
char* map - pointer to the map 

Output:
char* (result) - the resulting map with \n symbols, ready for dysplay

The function takes in map as a character sting with no \n, and inserts them after
each line so the map can be printed in client without extra calculations. 

*/
char* map_decode(char* map, game_t* game)
{
    // Calculate the length of the new string including newlines and null terminator
    int newLength = game->encodedMapLength + game->mapHeight; // Each row gets a newline
    char* result = mem_malloc((newLength + 1) * sizeof(char)); // +1 for the null terminator
    
    int resultIndex = 0;
    for (int i = 0; i < game->encodedMapLength; i++) {
        result[resultIndex++] = map[i];
        
        // If we've reached the end of a row, add a newline
        if ((i + 1) % game->mapWidth == 0) {
            result[resultIndex++] = '\n';
        }
    }
    result[resultIndex] = '\0'; // Null-terminate the string

    return result;  // Return the formatted map string
}
