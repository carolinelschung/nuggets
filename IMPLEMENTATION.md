## Implementation Spec
### Team 1-10, Fall, 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

#### Server Side:

- Nafis 
    - `Server Module` and Messaging
        - Handles messaging between server side and client side
- Joe 
    - `Game Module`
        - Handles game initialization
        - Handles game state updates whenever literally anything happens from any client
- Anya
    - `Map Module`
    - Map conversion from .txt to array and back
Set of functions that manage maps: take in a map, a command, and some player info return a new map 

#### Client Side:

- CC
    - `Client Module`
Can remove gameStatus
Make sure to handle mal messages

## Client Module
### Data Structures
Holds the primary client state, including server details, game status, and player symbol.
```c
typedef struct client {
  addr_t server;
  char* statusLine;
  char playerSymbol;
  bool isSpectator;
  bool quitting; 
  char quitMessage[7000];
} client_t;
```

### Definition of function protoytypes and Pseudocode
------------

##### `void parseArgs(client_t* client, int argc, char* argv[])`
Validates command-line arguments for client and configures initial state based on the type (player or spectator).
```c 
Check arguments length and parse hostname, port, and optional username
If player, send "PLAY <username>" to server
If spectator, send "SPECTATE" to server
```

##### `bool handle_server_message(void* arg, const addr_t from, const char* message)`
Processes messages from the server to update the client state or display relevant information.
```c
If "GRID", set up grid dimensions
If "GOLD", update player’s gold and status
If "DISPLAY", update client’s map view
If "QUIT", display quit message and stop loop
```

##### `bool handle_client_input(void* arg)`
Processes keystrokes from the user, sends them to the server, and handles quit commands.
```c 
Get keystroke input from player
If 'Q', send quit command
If valid movement key, send move to server
```
Each type of server message has its own helper function dedicated to it.

##### `void initialize_display()`
Sets up the display for the game, ensuring window dimensions are sufficient.
```c 
Set display mode to accept keystrokes without enter key
Suppress echo to avoid displaying keystrokes
Verify display dimensions for the game grid
Clear screen if dimensions are sufficient
```

## Server

### Definition of function prototypes & Pseudocode

##### `FILE* parseArgs(int argc, char* argv[], int* seed, int* gold, int* minPiles, int* maxPiles, bool* plain)`
Parses command-line arguments and initializes game parameters such as gold, map file, and player settings.
```C
Parse options for gold, min/max piles, seed, and map filename.
If no map file is provided, print usage and exit.
Open map file and return pointer to file handle.
```

##### `bool handleInput(void* arg)`
Handles server-side input commands such as status checks and quit.

```C
If input is "quit", print shutdown message and exit loop.
If input is "status", print server status message.
```

##### `bool handleMessage(void* arg, const addr_t from, const char* buf)`
Handles incoming messages from clients and updates game state accordingly.

```C 
If message is "PLAY", initialize player with unique letter and send grid and gold status.
If message is "SPECTATE", handle spectator replacement if necessary.
If message is "KEY <movement>", check validity of move and update game state.
If game is over, send final scores to players and spectators.
```
---

## Game module

### Data structures
`Game Structure`
Represents a game and all real time game data;
```C
typedef struct game {
    char* map;                    // Encoded game map
    char* mapWithNoPlayers;       // Map without player locations
    int mapWidth, mapHeight;      // Dimensions of the map
    int encodedMapLength;         // Length of the encoded map string
    int goldRemaining;            // Total gold left in the game
    int minGoldPiles, maxGoldPiles; // Minimum and maximum number of gold piles
    int activePlayersCount;       // Number of active players
    bool plain;                   // Plain mode flag
    hashtable_t* players;         // Hashtable of players by address
    addr_t spectatorAddress;      // Address of the spectator (if any)
    bool hasSpectator;            // Indicates if there's a spectator
    addr_t activePlayers[26];     // Array of player addresses (A-Z)
    char playerLetters[26];       // Array of player identifiers (A-Z)
    char nextAvailableLetter;     // Next available letter for player assignment
} game_t;
```

`Player Structure`
Represents a player in the game, including position, map, and gold information
```C
typedef struct player {
    char* playerName;             // Player's chosen name
    char* playerMap;              // Player's view of the map
    addr_t address;               // Player's network address
    char playerLetter;            // Player's letter identifier
    int xPosition, yPosition;     // Player's coordinates
    int goldCaptured;             // Total gold captured by player
    int goldJustCaptured;         // Gold captured in the last move
    bool isActive;                // Indicates if the player is active
} player_t;

### Definition of function prototypes & Pseudocode

##### `game_t* game_init(FILE* mapFile, int seed, int gold, int minGoldPiles, int maxGoldPiles, bool plain)`
Initializes the game, including the map, players, and gold placement.
```C
Seed the random generator based on seed or process ID.
Initialize player letter array and map by encoding map from the file.
Randomly place gold on the map within min/max pile constraints.
Return initialized game object.
```

##### `bool game_playerMove(addr_t playerAddress, game_t* game, char moveType)`
Moves player to a new location if the move is valid and updates gold if collected.
```C
Retrieve the player’s current position from `playerAddress`.
Check if the desired move is valid based on map boundaries and obstacles.
If valid:
    - Update player position and adjust map accordingly.
    - If the player collected gold, update gold count for the player and the game.
Refresh all players’ views of the map.
Return true if move was successful; false otherwise.

```

##### `bool validateAndMove(game_t* game, player_t* player, int proposedX, int proposedY)`
Checks if a proposed move is valid, updates the map, and handles interactions like gold collection or player-to-player collisions.
```C
Retrieve the tile at `proposedX` and `proposedY`.
If tile is not walkable, return false.
If tile contains gold, update player and game gold counts.
If tile contains another player and game is not in plain mode, handle player collision.
Update map with the player’s new position and return true.
```

##### `void game_placeGold(game_t* game)`
Randomly places gold piles across the map, ensuring the number of piles and total gold meet specified constraints.
```C
Calculate the number of gold piles between minGoldPiles and maxGoldPiles.
Initialize each pile with at least one gold.
Randomly distribute remaining gold among piles.
For each pile:
    Randomly select a walkable tile ('.') on the map and place the gold.
Update game’s goldRemaining to reflect total placed gold.
```

##### `player_t* game_playerInit(game_t* game, addr_t address, char* playerName)`
Initializes a new player and assigns them a unique letter and starting position on the map.
```C
Allocate memory for the new player struct.
If game has reached the max player limit (26 players), return NULL.
Assign the next available letter to the player.
Set playerName, xPosition, and yPosition.
Update game->activePlayers with the player’s address and letter.
Initialize player’s view of the map and mark their starting position.
Return the initialized player struct.
```

##### `char* game_getFinalScores(game_t* game)`
Compiles and formats the final scores of all players for display at the end of the game.
```C
Allocate a message buffer for the final scores.
For each player (A to Z):
    Retrieve player’s captured gold count and format as "Player <letter>: <gold captured>".
Append each player’s score to the message buffer.
Return the formatted final scores string.
```

##### `void game_delete(game_t* game)`
Cleans up and frees all allocated resources associated with the game.
```C
Free memory for the game map and mapWithNoPlayers.
For each player in the hashtable:
    Free player-specific memory such as name and map.
Free the players hashtable and other dynamically allocated resources.
Free the game structure itself.
```

##### `char* encodeMap(FILE* mapFile, game_t* game);`
Encodes the map from the provided file, setting the game’s map dimensions and storing it as a single string.
```C
Open map file and initialize map buffer.

For each line in map file:
    Set map width based on the first line's length.
    Ensure each line has consistent length with map width.
    Append line to the map buffer and update map height.

Return the encoded map as a single string.
```

##### `player_t* getPlayerByLetter(char letter, game_t* game);
Finds and returns a player structure based on their assigned letter identifier.`
```C
Iterate through players in the hashtable.
If player’s letter matches the given letter, return the player structure.
Return NULL if no matching player is found.
```
---

## Map module

The Map module in the Nuggets game handles the creation, visibility, and management of the in-game map. The map is stored as a character array, and each player has a personal view (visible map) of the full map (master map). This module manages players' starting positions, what each player can see, and updates player-specific maps as they explore or interact with the game environment.

### Definition of function prototypes & Pseudocode

##### `void map_player_init(char* masterMap, int* x, int* y, int* seed, const int NC, const int NR, game_t* game);`
Initializes a player’s starting coordinates on the master map based on the given seed. Ensures the player starts on an empty tile ('.').
```C
while (x, y) location is not a valid '.' on masterMap
    if seed is non null 
        x = srand(seed)
        y = srand(seed)
    else 
        x = srand(getpid())
        y = srand(getpid())
return
```


##### `void map_get_visible(int x, int y, char* masterMap, char* visibleMap, const int NC, const int NR);`
Calculates the visible area around a player’s current coordinates (x, y) based on line-of-sight.
```C
for each character on the masterMap (0 to strlen(masterMap))
    calculate coordinates of the point in (ptX, ptY) form
    if isVisible(x, y, ptX, ptY, masterMap)
        visibleMap(i) = masterMap(i);
    else 
        visibleMap(i) = ' ';
```

##### `static bool isVisible(int x, int y, int ptX, int ptY, char* masterMap, const int NC);`
Determines whether a given point (ptX, ptY) is visible from (x, y) using line-of-sight. This helper function is called within map_get_visible.
```C 
calculate slope coefficient between the two point = k (i.e. rise/run)
for(int i = 1; i < |x - ptX|; i++)
    yNew = x + i*k (round this value down)
    xNew = x + i
    if both masterMap(xNew, yNew) and masterMap(xNew, yNew+1) are non '.'/letter/gold
        return false
for(int i = 1; i < |y - ptY|; i++)
    xNew = Y + i*(1/k) (round this value down)
    yNew = y + i
    if both masterMap(xNew, yNew) and masterMap(xNew+1, yNew) are non '.'/letter/gold
        return false

return true
```

##### 'void map_merge(char* playerMap, char* visibleMap);'
Merges the player’s updated visible map into their overall player map, removing out-of-view gold or player markers.
```C
for each character on the masterMap (0 to strlen(playerMap))
    erase gold or other player:
    if playerMap(i) == golr or other letter 
        playerMap(i) = '.'
    insert whatever is coming from the new visible map:
    if visibleMap != ' '
        playerMap(i) = visibleMap(i)
```

##### 'char* map_decode(char* map, game_t* game);'
Converts a single string representation of the map into a displayable format by adding newlines at the end of each row.
```C
Calculate newLength for the formatted map string.
Allocate memory for the formatted map.
Loop through each tile in map:
    Append tile to result.
    Add newline after every row (based on mapWidth).
Null-terminate and return the formatted result.
```
---
## Extra Credit

#### Server Argument Options, including `--plain`
We have added the following optional arguments for the server:
- `--gold 500`: specifies the total gold in the game.
- `--minpiles 15`: sets the minimum number of gold piles.
- `--maxpiles 40`: sets the maximum number of gold piles.
- `--seed 123`: provides a seed for random generation of gold placement and player spawning
- `--plain`: allows the server to disable all extra credit capabilites.

#### Gold Stealing when Players Swap Spots
Players can steal gold by stepping onto other players. If a player enters the tile of another player, they take all gold the other player has collected. 

#### Drop Gold on Quit
When a player quits before the game ends, their collected gold is left at their last location on the map. This gold pile becomes available for other players to collect, encouraging strategy around player interactions.

---

## Testing plan

### Unit testing

- `Game Module`
    - Initialization: Test different game settings to check that the game initializes with correct map dimensions, player letters, and gold placement.
    - Player Movement: Test various player moves, ensuring valid moves update the player position and map, and invalid moves are rejected.
    - Gold Placement: Check if the correct amount of gold piles is placed only on walkable tiles.
    - Player Initialization: Verify that each player gets a unique letter and starting position and that the max player limit (26) is respected.
    - Final Scores: Test final scores format and values with different player setups.
    - Map Encoding: Test different map files to confirm encoding and consistent map dimensions.

- `Map Module`
    - Player Starting Position: Check that players are placed on valid starting tiles based on a given seed.
    - Visibility Calculation: Verify visible area around a player’s position, ensuring line-of-sight works correctly.
    - Map Merging: Confirm that the player’s map view is updated properly as they explore.
    - Visibility Check: Test visibility of distant tiles, ensuring obstacles block line-of-sight.

- `Client Module`
    - Argument Parsing: Test valid and invalid client command-line inputs to ensure proper configuration.
    - Server Message Handling: Check that the client processes different server messages correctly (e.g., grid setup, player status).
    - User Input Handling: Test user commands (e.g., movement and quit) to confirm expected behavior.

- 'Server Module'
    - Argument Parsing: Validate command-line inputs, ensuring correct game settings and map file handling.
    - Client Messages: Test different client messages to verify server responses and game state updates.
    - Input Commands: Test server input commands like "status" and "quit" to ensure expected server behavior.

### Integration testing

- `Server` and `Game` Modules
    - Server Startup: Test if server initializes the game correctly with different inputs.
    - Client Connections: Connect multiple clients provided support, verifying game responses and updates.
    - Game Events: Simulate/manually cause player movements and gold collection, confirming updates across all clients.
    - Game End: Test final scores are sent correctly to players at game end.

- `Client` and `Server` Communication
    - Message Protocols: Test message exchange for each command to confirm consistency.
    - Spectator Mode: Verify spectator view is updated without affecting gameplay.
    - Edge Cases: Test multiple simultaneous connections and rapid commands to check stability.

### System testing

- Full Gameplay Testing: Run complete game scenarios with multiple players and spectators, ensuring accurate score tracking and smooth gameplay flow.
- Real-Time Updates: Verify that spectators and players receive real-time map and score updates throughout the game.
- Error Handling: Simulate player disconnects, invalid inputs, and abrupt server shutdowns to confirm stability.
- Performance and Load Testing: Test server performance under maximum players and spectators, checking for responsiveness and memory leaks.
- End-to-End Tests: Conduct game sequences from setup through final scoring, confirming correct behavior and no crashes under typical gameplay scenarios.

---

## Limitations

