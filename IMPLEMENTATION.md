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
<<<<<<< HEAD
    - readMessage Module
    - sendMessage Module
=======
    - Message Module
        - Handles messaging between server side and client side
>>>>>>> 4b1105c (Implementation specifications updated)
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
<<<<<<< HEAD

=======
>>>>>>> 4b1105c (Implementation specifications updated)

## Player
### Data Structures
##### Client
'Client_start' holds basic info about the user, including the hostname of the server, the port number, the username of the client (if they are using one), and if they are a spectator
```c
typedef struct {
    char* hostname;       
    int port;              
    char* playerName;  
    bool isSpectator;    
} client_start_t;
```

##### Client Game State
Holds information about the current game state, collected via messsages from server.  Information includes: number of rows and columns in the grid, amount of gold collected by player and remaining in the map, the display grid as a string, and the player symbol on the screen
```c
typedef struct {
    int num_rows;            
    int num_cols;            
    int goldCollected;     
    int goldRemaining;     
    char* display;  
    char* statusLine;   
} client_game_state_t;
```

### Definition of function protoytypes
------------
##### initialize_client
The initialize_client function performs the following:
1. Inititalizes logging and messaging modules.
2. Validates command line arguments with parseArgs
3. Sets up the server address
4. Either sends 'PLAY' or 'SPECTATE' message
5. Allocates memory for the game status line
```c 
bool initialize_client(client_t* client, client_game_state_t* state, char* serverAddress[], FILE* logFP, int argc, char* argv[]);
```

##### parseArgs
Validates command line arguments:
1. Checks to make sure command-line arguments are provided.
2. If playername is not provided, then isSpectator is true, and playername is empty, otherwise playername is set and isSpectator is false.
```c 
bool parseArgs(client_start_t* client, int argc, char* argv[]);
```

##### initialize_display
Sets up the initial display and checks that the window is sufficiently large based on GRID message from server.
```c
bool initialize_display(client_game_state_t* state, int num_rows, int num_cols);
```

##### handle_server_message
Processes messages from server in message_loop based on 'GRID', 'GOLD', 'DISPLAY', 'QUIT', 'KEY', and 'OK'
```c 
bool handle_server_message(client_game_state_t* state, const char* fromAddress, message_t* message);
```
Each type of server message has its own helper function dedicated to it.

##### handle_client_input
Interpret keystrokes from stdin.
```c 
bool handle_client_input(client_game_state_t* state);
```

----

### Detailed pseudocode
------
##### initialize_client
```text 
initialize log
set server addresss
send PLAY or SPECTATE message
allocate memory for status line
```

##### parseArgs
```text 
// validate commandline
if argc < 3:
    print to stderr "Usage: ./client hostname port [username (optional)]"
    exit(nonzero)
// initialize message module 

client->hostname = argv[1]

call message_init

if argv[2] > 0:
    client->port = argv[2]
    print client port number
else:
    print to stderr "Error: Invalid port number"
    exit(nonzero)

// determine if player or spectator
if argc == 4:
    client->isSpectator = false
    validate player name length
    client->playerName = argv[3]
else:
    client->isSpectator = true
    client->playerName = ""
```

##### initialize_Display
```text
set state's num_rows and num_cols
if window size < needed size:
    print "Error: Window size too small for grid"
    return false
allocate memory for display based on grid size
Update dispay to show empty grid

return true
```

##### handle_server_message
```text 
if message starts with 'GRID':
    parse grid dimensions
    update state->num_rows
    update state->num_cols
    call initialize_display with state 
if message starts with 'GOLD':
    update state->goldCollected
    updated state->goldRemaining
    update statusLine
if message starts with 'DISPLAY':
    copy message to display
if message starts with 'OK':
    log valid move
if message starts with 'QUIT':
    print quit message
    clean up
if message starts with 'ERROR':
    add error message to status line
```

##### handle_client_input
```text 
get keystroke from user
if key pressed is 'Q':
    send 'KEY Q' to server
if key is a valid movement key:
    send 'KEY <movement key>' to server
```

### Testing Plan
##### Unit Testing
Test each of the functions seperately to verify their correct behavior.
1. initialize_client & parseArgs:
    * Verify that the initialize_client and parseArgs functions correctly handle valid and invalid inputs
    * Test cases:
        * Parses hostname, port, and playername (optional)
        * Handles missing arguments and invalid arguments (like negative port number) by printing error and exiting
        * Prints error message and exits when encoutering a playername that is too long
2. handle_message:
    * Verify that handle_message responds as expected to each server message type
    * Test cases:
        * GOLD messages: Parses grid message and initializes display with the given dimensions
        * GOLD messages: Updates statusLine with player's current nugget count and unclaimed nuggets
        * DISPLAY messages: Updates the display by printing the new display message
        * QUIT messages: Exits gracefully and prints exit message
        * ERROR messages: logs error message and adds to statusLine
        * OK messages: logs valid messages
3. initialize_display:
    * Verify that inititalize_display initializes diapl correctly based on grid size
    * Test cases:
        * Check for valid grid sizes
        * Check for successful memory allocation
4. handle_input:
    * Verify that handle_input correctly processes keystrokes and sends them to server in proper format
    * Test cases:
        * Valid movement keys send correct KEY messages to the server
        * Invalid keys are logged as errors 

##### Regression Testing
Check to make sure game continues to work properly over time. Use a testing script to run games of varying lengths, including errors like invalid keys, invalid server messages, etc.

##### Integration Testing
Run a testing.sh that runs the client. First runs with a sequence of tests for invalid arguments, then a run of a full game with valgrind to test for leaks.  Then integrate with server by mimicking server messages, valid and invalid to confirm correct server message processing.

## Server

### Data structures


### Definition of function prototypes



A function to parse the command-line arguments, initialize the game struct, initialize the message module, and initialize analytics module.

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
<<<<<<< HEAD
=======
    int mapLength;
    int mapWidth;
>>>>>>> 4b1105c (Implementation specifications updated)
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
<<<<<<< HEAD
=======
    char* address;
>>>>>>> 4b1105c (Implementation specifications updated)
    char role;
    int xPosition;
    int yPosition;
    int goldCaptured;
    bool isActive;
} player_t;
```
### Definition of function prototypes

<<<<<<< HEAD
`initializeGame`
=======
`game_init`
>>>>>>> 4b1105c (Implementation specifications updated)
Initializes the game state, including map, players, and gold piles. 
```C
int initializeGame(const char* mapFile, int seed);
```

<<<<<<< HEAD
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
=======
`game_updateState`
Updates the game state based on player actions or events.
```C
void updateState(player_t* player, char action);
```

`game_playerMove`
Modifies the encoded map string to move the player character to a requested location only if move is valid. Also checks if player moves onto gold. Returns new encoded visible map.
```C
char* game_playerMove(char* currentVisibleMap, char moveType, int xPos, int yPos);
```

`game_mapEncode`
Function that turns a .txt file into a string.
`char* game_mapEncode(FILE* fp);`
>>>>>>> 4b1105c (Implementation specifications updated)

### Detailed pseudo code

`game_init`
```
encode map to string from mapfile
place gold piles randomly on map
initialize player hashtable in global game object
initialize active players array in global game object
intialize gold
```

`game_updateState`
```
receive player action
switch (action):
    case move:
        playerMove
    case quit:
        deactivate player in players hashtable
        remove player from active players array
    if goldRemaining is zero:
        end game and show results
```

`game_playerMove`
```
Go to current position in map string of player given xPos and yPos
switch (moveType):
    case up:
        backtrack through string by width of map and check if character is valid to move to
        if valid:
            update player pos
            check if player collected gold
            if collected gold:
                increment players gold
                decrement gold remaining
            refresh all players visible map
    case down:
        go forward through string by width of map and check if character is valid to move to
        if valid:
            update player pos
            check if player collected gold
            if collected gold:
                increment players gold
                decrement gold remaining
            refresh all players visible map
    case left:
        Check if character to left is valid to move to
        if valid:
            update player pos
            check if player collected gold
            if collected gold:
                increment players gold
                decrement gold remaining
            refresh all players visible map
    case right:
        Check if character to right is valid to move to
        if valid:
            update player pos
            check if player collected gold
            if collected gold:
                increment players gold
                decrement gold remaining
            refresh all players visible map
        
```

`char* game_mapEncode(FILE* fp);`

```C 
open file
malloc a map string with mapWidth*mapHeight characters 
while not EOF read file 
    put character into the string
return map string
```




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
`char* map_encode(FILE* fp, int mapWidth, int mapHeight);`

Function that takes in seed and based on it returns a player's starting coordinates on the masterMap.
`void map_player_init(char* masterMap, int* x, int* y, int* seed);`

Function that takes in a player's coordinates and the master map and outputs the visibleMap (only what the payer sees immediately)  
`void map_get_visible(int x, int y, char* masterMap char* visibleMap);`

Function that takes in the visible map and the payer's presious map and merges them omitting the gold from previous map. 
`void map_merge(char* playerMap, char* visibleMap);`


### Detailed pseudo code


`void map_player_init(char* masterMap, int* x, int* y, int* seed);`

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
`void map_get_visible(int x, int y, char* masterMap char* visibleMap);`

```C
for each character on the masterMap (0 to strlen(masterMap))
    calculate coordinates of the point in (ptX, ptY) form
    if isVisible(x, y, ptX, ptY, masterMap)
        visibleMap(i) = masterMap(i);
    else 
        visibleMap(i) = ' ';
```

Helper functoin for getting visible map 
`bool isVisible(int x, int y, int ptX, int ptY, char* masterMap)`

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

`void map_merge(char* playerMap, char* visibleMap);`

```C
for each character on the masterMap (0 to strlen(playerMap))
    erase gold or other player:
    if playerMap(i) == golr or other letter 
        playerMap(i) = '.'
    insert whatever is coming from the new visible map:
    if visibleMap != ' '
        playerMap(i) = visibleMap(i)
```
---
### Message Module

We have been provided a ```message``` module and to utilize that, we write the following functions

```C 
static bool handleTimeout(void* arg);
```
```handleTimeout``` handles the situation if there's no input within the specified time

```C
static bool handleMessage(void* arg, const addr_t from, const char* message, player_t* player);
```
```handleMessage``` handles any incoming message and takes necessary actions

```C 
static bool handleInput (void* arg);
```
```handleInput``` takes the input of the player and sends that to the server.

### Pseudocode for handleInput
```
if there's no input
    log null
else
    send the message to the server using message_send
```
### Pseudocode for handleTimeout
```
if there's no input
    log null
else
    send a message to the player
```
### Pseudocode for handleMessage
```
if there's not proper argument
    log null
else
    call update game state using the appropriate message
```
## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### Integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### System testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.