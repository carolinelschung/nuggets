## Implementation Spec
### Team name, term, year

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

#### Server Side:

- Nafis 
    - readMessage Module
    - sendMessage Module
- Joe 
    - Game Module
        - Handles game initialization
        - Handles game state updates whenever literally anything happens from any client
- Anya
     - Map module
    - Map conversion from .txt to array and back
Set of functions that manage maps: take in a map, a command, and some player info return a new map 

#### Client Side:

- CC
    - readMessage Module
    - sendMessage Module
Can remove gameStatus
Make sure to handle mal messages


## Player

> Teams of 3 students should delete this section.

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	initialize message module
	print assigned port number
	decide whether spectator or player

---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## Game module

### Data structures

#### Game Struct

```C
typedef struct game {
    char* map;
    hashtable_t* players;
    player_t* activePlayers[];
    int goldRemaning;
} game_t;
```

#### Player Struct
```C
typedef struct player {
    char* playerName;
    char* playerMap;
    char role;
    int xPosition;
    int yPosition;
    int goldCaptured;
    bool isActive;
} player_t;
```
### Definition of function prototypes

`initializeGame`
Initializes the game state, including map, players, and gold piles. 
```C
int initializeGame(const char* mapFile, int seed);
```

`updateGameState`
Updates the game state based on player actions or events.
```C
void updateGameState(player_t* player, char action);
```

`move`
Modifies the encoded map string to move the player character to a requested location only if move is valid (no wall, no deadend, etc). Also checks if player moves onto gold. Returns new encoded visible map.
```C
char* move(char* currentVisibleMap, char moveType, int xPos, int yPos);
```

### Detailed pseudo code

`initalizeGame`
```
encode map to string from mapfile
place gold piles randomly on map
initialize player hashtable in global game object
initialize active players array in global game object
intialize gold
```

`updateGameState`
```
receive player action
switch (action):
    case move:
    check if move is valid:
    if valid:
        update player pos
        check if player collected gold
        if collected gold:
            increment players gold
            decrement gold remaining
        refresh all players visible map
    case quit:
        deactivate player in players hashtable
        remove player from active players array
    if goldRemaining is zero:
        end game and show results
```

---


---


## Map module

The module will utilize previously specified player_t struct.
The idea of the module is to handle changes to what each client who is plyaing sees.

Important definitions for this module's description:

playerMap - the map the client sees on their computer
visibleMap - the map the player sees at one time (i.e. not inclusing previously seen rooms etc., equivalent to the region where a player would see gold)
masterMap - the full game map with up to date player positions and gold positions, taken care of in game module 

### Data structures

```C
typedef struct player {
    char* playerName;
    char* playerMap;
    char role;
    int xPosition;
    int yPosition;
    int goldCaptured;
    bool isActive;
} player_t;
```

### Definition of function prototypes

Function that turns a .txt file into a string.
`char* map_encode(FILE* fp);`

Function that takes in seed and based on it returns a player's starting coordinates on the masterMap.
`void map_player_init(char* masterMap, int* x, int* y, int seed);`

Function that takes in a player's coordinates and the master map  
`void map_get_visible(int x, int y, char* masterMap char* visibleMap);`

### Detailed pseudo code

---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.