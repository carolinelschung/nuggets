---
title: README

---

# Map module of the Nuggets project
## Team 10, Anna Filyurina, Nov. 2024
(Map_decode functino contributed by Joseph Quaratiello)

The module is responsible for visibility of the map. It has 4 functions:

```c
void map_player_init(char* masterMap, int* x, int* y, int* seed, const int NC, const int NR, game_t* game);

void map_get_visible(int x, int y, char* masterMap, char* visibleMap, const int NC, const int NR);

void map_merge(char* playerMap, char* visibleMap, int NC, int NR);

char* map_decode(char* map, game_t* game);
```

`map_player_init` is called for every player indivisually in order to pick 'random' corrdinates for their placement and add the `@` symbol to the map.

`map_get_visible` is called after player initialization, to get their first visible map, and is called at every map change in order to compute player's immediate field of view.

`map_merge` is called on every map change and it combines the player's previous map with the player's new visible map.

`map_decode` inserts `\n` symbols into the map so that the client can print it, also called on every map change. 

Any string that is passes into the module is expected to be initialized and the memory is expected to be already allocated. Apart from `map_decode()`, the module does not `malloc()` or `free()` any memory.

IMPORTANT:

The string returned from `map_decode()` is expected to be freed by the user. 