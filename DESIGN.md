# CS50 Nuggets
## Design Spec
### Team 1-10, 24F

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Player

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI. 

### Inputs and outputs

| Key Command | Action |
| -------- | -------- |
| Q     | quit the game |
| h     | move left, if possible |
| l     | move right, if possible |
| j     | move down, if possible |
| k     | move up, if possible |
| y     | move diagonally up and left, if possible |
| u     | move diagonally up and right, if possible |
| b     | move diagonally down and left, if possible |
| n     | move diagonally down and right, if possible |

> If you write to log files, or log to stderr, describe that here.
- Game start, game end 
- Message whenever a new player/spectator joins
- Message whenever a player leaves or a spectator gets kicked off 
- Message whenever gold gets collected 

> Command-line arguments are not 'input'.

The first argument is the hostname or IP address where the server is running. The port number is where the server expects messages. If optional playername used, client joins as a player; otherwise, client joins as spectator.
```bash
./client hostname port [playername]
./client 2>player.log hostname port playername
./client 2>spectator.log hostname port
```

### Functional decomposition into modules

#### Parsing Arguments
Arguments take whether it's player or spectator, if no username is provided, automatically assumes spectator. initializes message structure.

#### User Interface
Displays game info including map, number of nuggest uncollected, number of nuggets collected (if player), number of spectators, and error messages.

#### Client/Server Communication
Handles processing messages from server, sends messages to server to update game.

#### Update Game
Changes the status of the game as required, i.e. changing the number of nuggets collected, changing the position of the player and refreshing the screen
 
### Pseudo code for logic/algorithmic flow
#### parseArgs
Checks validity of commandline arguments, initializes message module, and checks if user is a spectator or a player.
```text 
validate commandline
initialize message module
print assigned port number
decide whether spectator or player
```

#### sendMessage
Sends message to server based on user input. Based on user input case will send message to server
```text
if key pressed is:
    case 'movement key':
        handle movement
    case 'Q':
        handle quit
```

#### handleMessage
Processes message from server.
```text 
if first word is:
    case 'play':
        handle play
    case 'ok':
        handle ok
    case 'grid':
        handle grid
    case 'gold':
        handle gold
    case 'key':
        handle key
```

#### updateGame
Accepts parameters from the handle functions, takes that information and uses it to refresh the screen of the player after receiving message from server.
```text 
receive parameters from handle functions
clears screen
prints new, updated map
display scores
```

#### main 
Handles parsing args, initialization, connecting to server, processing messages to and from server, and clean up when game is over.
```text
parse command-line args
initialize client as player or spectator
connect to server
load initial map - if its too small, print error 
if player:
    while game is not over:
        accept inputs 
        send inputs to server
        handle messages from server
        update screen
if spectator:
    while game is not over:
        handle messages from server
        update screen
once game is over:
    clean up memory allocs
```

### Major data structures

#### Message 
stores and handles message functionality between client and server. Handles errors, socket connections, and clean up once game is over

#### Player
Player stores information about the user, including:
    * whether they are a player or spectator
    * name/id
    * x and y coordinates on the map
    * total number of nuggets collected
    
#### Game Status
For purposes of making data visualization easier, keeps track of:
    * list of players
    * number of spectators
    * amount of gold remaining in map
> A language-independent description of the major data structure(s) in this program.
> Mention, but do not describe, any libcs50 data structures you plan to use.

---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

Server has a to recieve a .txt file of the map, it can optionally take a seed that determines starting location and an argument for outputting logs to a file.

```bash
./server map.txt [seed]
./server 2>server.log map.txt
```

### Functional decomposition into modules

`parseArgs` verifies `./server` arguments. If error, provide a useful error message and exit non-zero. Vertifiy map file can be opened for reading.
 
`generateLocation` sets initial location on the map. An optional seed is provided, the server shall pass it to `srand(seed)`. If no seed is provided, the server shall use `srand(getpid())` to produce random behavior.

`gridLoad` take the map file and turns it into useable 

`initializeGame`
- Initialize the game by dropping at least GoldMinNumPiles and at most GoldMaxNumPiles gold piles on random room spots; each pile shall have a random number of nuggets.
- Initialize players hashtable
- Initialize active players array
- Initialize the network and announce the port number.

`processMessage`
- If message_loop returns true, do everything below
- Process connecting user as player/spectator
    - Add player object to player array
    - Init location
- Process quit
- Accept up to MaxPlayers players; if a player exits or quits the game, it can neither rejoin nor be replaced. Thus: once a player has quit, its letter is never re-used.
- Accept up to 1 spectator; if a new spectator joins while one is active, the server shall tell the current spectator to quit, and the server shall then forget that current spectator.
- React to each type of inbound message as described in the protocol below.
Handle errors, including malloc failures, gracefully.
- If a player quits the game, that player's symbol is removed from the map.
- Update all clients whenever any player moves or gold is collected.

### Pseudo code for logic/algorithmic flow


```
execute from a command line per the requirement spec
parse the command line with parseArgs, validate parameters
call initializeGame() to set up data structures
initialize the 'message' module
print the port number on which we wait
while a new message from client recieved
    if movement command recieved 
        update player map
        update main map (in case gold was eaten)
        iterate through active players 
            if moved player was visible update other players' view
    if player quit 
        iterate through active players 
            if deleted player was visible update other players' view
        update hashtable with players (chnge status)
        delete the player pointer from active player list
    if player joins
        initilize a player_t struct for them
        iterate through active players 
            if added player was visible update other players' view
    if all gold was collected leave loop
call gameOver() to inform all clients the game has ended
clean up
```


### Major data structures

- Game Struct (Global Variable)
    - Map String
        - Map txt file will be converted to a string. We will know dimensions of map for purpose of decoding string into useable position that are empty, walls, have gold, etc.
    - Players Hashtable
        - Key: IP Address
        - Item: Player_t struct
             - playerName
             - visibleMap
             - role
             - xPosition
             - yPosition
             - goldCaptured
             - status
    - Active Players Array
        - Array of pointers to player_t struct
        - Makes it easy to simultanously keep track of active users and quickly access all user metadata
    - goldCount: int for running gold count


Store map as an array a struct map_t:
![77E0A342-2493-4FC6-A8B5-4775B2A0B0BF](https://hackmd.io/_uploads/ryz26RPWJe.png)
