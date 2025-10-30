# CS50 Nuggets
## CS50 Fall 2024

### based on the bag README.md

### Client Module for the Nuggets Game

This module implements the client-side functionality for the Nuggets game.  The client connects to a server as either a player or spectator.  The user's keystrokes are taken and sent to the server as KEY messages and server messages are processed to update the display of the client.

### Usage

The client module is defined in client.c and includes the following key functions:

```c
bool initialize_client(client_game_state_t* state, addr_t* serverAddress, FILE* logFP, int argc, char* argv[]);
void parseArgs(client_init_t* client, int argc, char* argv[]);
bool initialize_display(client_game_state_t* state, int num_rows, int num_cols);
bool handle_server_message(void* arg, const addr_t from, const char* message);
bool handle_client_input(void* arg);
```

### Implementation

The client module uses ncurses to create the user interface that displays the map, player statistics, and status messages. The client connects to the server and communicates via messafesm updating its game state based on server responses and user input.

#####Functions:
* initialize_client() - The initialize_client function initializes the client and connects to the server. Sends a PLAY message if the client is a player or SPECTATE if they are a spectator
* parseArgs() - Validates command-line arguments, determines if the client is a player or a spectator, and populates client_init_t struct
* initialize_display() - Sets up the display screen for the game map based on dimensions received from the server
* handle_server_message() - Processes messages from server and calls appropriate handling functions
* handle_client_input() - Sends keystrokes from the client to the server in a KEY message

### Client Commands
Once the client is connected to the server, the client can make these commands using these keyboard inputs:
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

### Assumptions
* Player names may not exceed 50 characters.
* Messages may not exceed 65507 characters.
* Messages will arrive from server in valid condition to be parsed.

### Files
* `Makefile` - compilation procedure
* `client.h` - the interface
* `client.c` - the implementation

### Compilation

to compile, simply 'make'

