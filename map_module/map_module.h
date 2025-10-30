// Header file of map modlue for the Nuggets project
// CS50, 24F
// Detailed descriptions of each function in map_module.c
// Team 10, Anna Filyurina, Nov. 2024

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<math.h>



/*
Function that takes in seed and based on it returns a player's starting coordinates on the mainMap.
*/
void map_player_init(char* mainMap, int* x, int* y, int* seed, const int NC, const int NR);

/*
Function that takes in a player's coordinates and the main map and outputs the visibleMap (only what the payer sees immediately)  

*/
void map_get_visible(int x, int y, char* mainMap, char* visibleMap, const int NC, const int NR);

/*
Function that takes in the visible map and the payer's presious map and merges them omitting the gold from previous map. 

*/
void map_merge(char* playerMap, char* visibleMap);