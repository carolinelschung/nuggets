// Testing code of map modlue for the Nuggets project
// CS50, 24F
// Team 10, Anna Filyurina, Nov. 2024

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "map_module.h"
#include "../libcs50/mem.h"

void* mem_malloc(const size_t size);

#define MAX_LINE_LENGTH 1024


int main(){
  FILE* fp;
  int mapHeight = 0, mapWidth = 0;
  fp = fopen("../maps/hole.txt", "r");
  if(fp == NULL){
    printf("You can't spell the file name... try again \n");
  }else{
    printf("opened map file!\n");
  }

  // Initialize map string buffer
  char* map = mem_malloc(1); // Start with an empty string
  size_t mapSize = 0; 
  char line[MAX_LINE_LENGTH];
  // Read the map file line by line
  while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        size_t lineLen = strlen(line);

        // Remove newline character at the end of the line, if present
        if (line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';
            lineLen--;
        }

        // Set mapWidth based on the first line (excluding newline character)
        if (mapWidth == 0) {
            mapWidth = lineLen;  // Set width for all lines; excludes newline
        }

        // Allocate enough memory for the existing map and new line
        // Replace mem_realloc with realloc in loadMap function
        char* newMap = realloc(map, mapSize + lineLen + 1);

        if (newMap == NULL) {
            fprintf(stderr, "Error: Memory allocation failed while loading map.\n");
            free(map);
            fclose(fp);
            return 1;
        }
        map = newMap;

        // Append the current line to the map string
        memcpy(map + mapSize, line, lineLen);
        mapSize += lineLen;
        mapHeight++;  // Increment height for each line read
    }

    // Null-terminate the map string
    map[mapSize] = '\0';

    fclose(fp);

    printf("%s\n", map);

  printf("Player posistion initialization testing \n");
  int x;
  int y;
  int seed = 16;
  char* demo = mem_malloc(sizeof(char)*strlen(map));
  char* visible_1 = mem_malloc(sizeof(char)*strlen(map));
  char* visible_2 = mem_malloc(sizeof(char)*strlen(map));
  char* visible_3 = mem_malloc(sizeof(char)*strlen(map));
  char* visible_4 = mem_malloc(sizeof(char)*strlen(map));


  printf("got to func call\n");
  fflush(stdout);
  // map_player_init(map, &x, &y, &seed, mapWidth, mapHeight);
  x = 33;
  y = 14;
  printf("%d %d \n", x, y);

  strcpy(demo, map);
  demo[y*mapWidth + x + 1] = '@';
  printf("%s\n", demo);
  map[14*mapWidth + 25] = '*';
  map[13*mapWidth + 20] = 'C';


  map_get_visible(x, y, map, visible_1, mapWidth, mapHeight);
  visible_1[y*mapWidth + x + 1] = '@';
  printf("%s\n", visible_1);

  y = 15;

  map_get_visible(x, y, map, visible_2, mapWidth, mapHeight);
  visible_2[y*mapWidth + x + 1] = '@';
  printf("%s\n", visible_2);

  printf("Merged map after first step \n");
  map_merge(visible_1, visible_2);
  printf("%s\n", visible_1);

  y = 16;

  map_get_visible(x, y, map, visible_3, mapWidth, mapHeight);
  visible_3[y*mapWidth + x + 1] = '@';
  printf("%s\n", visible_3);

  printf("Merged map after second step \n");
  map_merge(visible_1, visible_3);
  printf("%s\n", visible_1);

  y = 17;

  map_get_visible(x, y, map, visible_4, mapWidth, mapHeight);
  visible_4[y*mapWidth + x + 1] = '@';
  printf("%s\n", visible_4);

  printf("Merged map after third step \n");
  map_merge(visible_1, visible_4);
  printf("%s\n", visible_1);

  // seed = 25;
  // map_player_init(map, &x, &y, &seed, mapWidth, mapHeight);
  // printf("%d %d \n", x, y);

  // strcpy(demo, map);
  // demo[y*mapWidth + x + 1] = '@';
  // printf("%s\n", demo);

  // map_get_visible(x, y, map, visible, mapWidth, mapHeight);
  // visible[y*mapWidth + x + 1] = '@';
  // printf("%s\n", visible);

  // seed = 100;
  // map_player_init(map, &x, &y, &seed, mapWidth, mapHeight);
  // printf("%d %d \n", x, y);

  // strcpy(demo, map);
  // demo[y*mapWidth + x + 1] = '@';
  // printf("%s\n", demo);

  // map_get_visible(x, y, map, visible, mapWidth, mapHeight);
  // visible[y*mapWidth + x + 1] = '@';
  // printf("%s\n", visible);

  return 0;
}

static int nmalloc = 0;         // number of successful malloc calls

/**************** mem_malloc() ****************/
/* see mem.h for description */
void*
mem_malloc(const size_t size)
{
  void* ptr = malloc(size);
  if (ptr != NULL) {
    nmalloc++;
  }
  return ptr;
}