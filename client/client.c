#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "ctype.h"
#include "message.h"
#include "log.h"
#include <ncurses.h>

#define MAX_NAME_LENGTH 50 // max number of chars in playerName
#define GOLD_TOTAL 250 // total amount of gold in the game
// based on line given in reguirements spec, 
// "Player A has 39 nuggets (211 nuggets unclaimed). GOLD received: 39"
// but adding 40 characters as a buffer
#define MAX_STATUS_LENGTH 100
// 26 * (MAX_NAME_LENGTH + 20) + 20 chars for end of game header 
#define MAX_QUIT_MESSAGE_LENGTH 1900

/**************** global types ****************/
// struct to hold necessary starting info for client to intitialize game
typedef struct client_init {
  char* hostname;
  int port;
  char* playerName;
  bool isSpectator;
} client_init_t;

// struct to hold current game state information
typedef struct client_game_state {
  int num_rows;
  int num_cols;
  int purseGold;
  int goldRemaining;
  char playerSymbol;
  char* display;
  char* statusLine;
  client_init_t* client;  
} client_game_state_t;

/************* function prototypes ************/


// /******************* main ********************/
// int main(int argc, char* argv[]) 
// {
//   client_init_t* client;
//   client_game_state_t* state;

//   initialize_client(client, unkwnonw, stderr, argv, argv);

//   initialize_display(state, )
// }

/******************* initialize_client *****************/
/*
 * initialize_client - inititalizes the connection to server, calls parse args to populate 
 * information in client, then sends either 'PLAY' or 
 * 'SPECTATE' message to server.
 *
 * Caller provides:
 *   client - client start struct to be populated with info from command line
 *   serverAddress - address of the server
 *   logFP - location of where relevant output goes 
 *   argc - the number of command line args
 *   argv - array of command line args 
 * Returns:
 *   True if initialization works.  False otherwise.
 */
bool initialize_client(client_game_state_t* state, addr_t* serverAddress, FILE* logFP, int argc, char* argv[])
{
  // initialize logging module
  log_init(logFP); 

  if (message_init(logFP) == 0) {
    fprintf(stderr, "Error: Failed to initialize message module\n");
    return false;
  } 

  // call parseArgs to fill in client_init info
  parseArgs(state->client, argc, argv);

  // create server address, max port numebr is 26, two digists plus one for a null char
  if (!message_setAddr(state->client->hostname, argv[2], serverAddress)) {
    fprintf(stderr, "Error: Unable to set server address\n");
    return false;
  }

  // initialize statusLine
  state->statusLine = malloc(MAX_STATUS_LENGTH * sizeof(char));
  if (state->statusLine == NULL) {
    fprintf(stderr, "Error: Unable to allocate memory for status line.\n");
    return false;
  }
  
  return true;
}

/******************* parseArgs *****************/
/*
 * parseArgs - Validates command line arguments.
 *
 * Caller provides:
 *   client - struct that holds all info needed about client to initialize game
 *   argc - number of command line args
 *   argv - array of command line args
 *     argv[1] - hostname
 *     argv[2] - port where the client will connect to server
 *     argv[3] - playername (optional)
 * Returns:
 *   nothing
 */
void parseArgs(client_init_t* client, int argc, char* argv[])
{
  // validate command line length
  if (argc < 3) {
    fprintf(stderr, "Usage: ./client hostname port <username> (username is optional)");
    exit(1); // invalid command line arguments
  }

  // set client hostname
  client->hostname = argv[1];  

  // check to make sure port is number larger than 0
  if (argv[2] > 0) {
    client->port = atoi(argv[2]);
    printf("Port %d was connected to.", client->port); //
  }
  else {
    fprintf(stderr, "Error: Invalid port number\n");
    exit(1); // invalid command line arguments 
  }

  // determine if player or spectator
  if (argc == 4) {
    client->isSpectator = false; // fourth arg indicates player                        
    client->playerName = argv[3];
    if (strlen(client->playerName) > MAX_NAME_LENGTH) {
      fprintf(stderr, "Error: Player name length may not exceed 50 characters\n");
      exit(1); // invalid command line args
    }
  }
  else {
    client->isSpectator = true;
    client->playerName = NULL;
  }
}

/******************* initialize_display *****************/
/*
 * initialize_display - sets up the display initially and checks for 
 * valid window size based on "GRID" message from server.
 *
 * Caller provides:
 *   state - struct that holds current game state
 *   num_rows - number of rows 
 *   num_cols - number of columns
 * Returns:
 *   True if initialization works.  False otherwise.
 */
bool initialize_display(client_game_state_t* state, int num_rows, int num_cols) 
{
  // set the number of rows and columns in the game state struct
  state->num_cols = num_cols;
  state->num_rows = num_rows;

  // dynamically allocate memory for the display based on the grid size
  state->display = malloc(num_cols * num_rows * sizeof(char));
  if (state->display == NULL) { // check for mem alloc failure
    fprintf(stderr, "Error: Unable to allocate memory for grid display\n");
    return false;
  }

  // fill display with " " chars at first as placeholder
  memset(state->display, ' ', num_cols * num_rows);
  return true;
}

/******************* handle_server_message *****************/
/*
 * handle_server_message - handles server messages, calling relevant
 * helper functions as necessary
 * 
 *
 * Caller provides:
 *   arg - struct that holds current game state
 *   from - number of rows 
 *   message - number of columns
 * Returns:
 *   True if message handling works.  False otherwise.
 */
static bool handle_server_message(void* arg, const addr_t from, const char* message)
{
  client_game_state_t* state = arg;

  if (strncmp(message, "GRID", 4) == 0) {
    handle_message_grid(state, message);
  }
  else if (strncmp(message, "GOLD", 4) == 0) {
    handle_gold_message(state, message);
  }
  else if (strncmp(message, "OK", 2) == 0) {
    handle_ok_message(state, message);
  }
  else if (strncmp(message, "QUIT", 4) == 0) {
    handle_quit_message(state, message);
  }
  else if (stncmp(message, "ERROR", 5) == 0) {
    handle_error_message(state, message);
  }
  
  return true;
}

/******************* handle_message_grid *****************/
/*
 * handle_message_grid - handles "GRID" server messages, updating client game state
 * and checking that the display is sufficiently large using helper function called
 * check_display_dimensions.
 * 
 * Caller provides:
 *   state - game state struct containing all current info 
 *   message - message from server
 * Returns:
 *   nothing
 */
void handle_message_grid(client_game_state_t* state, const char* message)
{
  int rows;
  int cols; 

  // update game state to have most curr rows and cols 
  sscanf(message, "GRID %d %d", &rows, &cols);
  state->num_cols = cols;
  state->num_rows = rows;

  // check to make sure the dimensions of the display are valid
  if (check_display_dimensions(rows, cols)) {
    // initialize the display with the appropriate amount of rows and cols
    initialize_display(state, rows, cols);
  }
  else {
    fprintf(stderr, "Error: Display dimensions are too small for the game.\n");
  }
}

/******************* check_display_dimensions *****************/
/*
 * check_display_dimensions - checks for valid dimensions of the display
 * compared to the number of rows and columns needed for the map
 * 
 * Caller provides:
 *   num_rows - number of rows needed for the map
 *   num_cols - number of columns needed for the map
 * Returns:
 *   True if display is sufficiently large.  False otherwise.
 */
static bool check_display_dimensions(int num_rows, int num_cols) {
  int max_rows; // the number of rows the display could show
  int max_cols; // the number of columns the display could show

  // get curr display size
  getmaxyx(stdscr, max_rows, max_cols); 

  if (max_rows < num_rows || max_cols < num_cols) {
    fprintf(stderr, "Error: Terminal size too small. Please resize to at least %d rows and %d column\n", num_rows, num_cols);
    return false;
  }

  return true;
}

/******************* handle_gold_message *****************/
/*
 * handle_gold_message - reads the relevant message from server, 
 * parsing message for n (amount of gold collected in pile), p 
 * (amount of gold in purse), and r (amount of gold remaining) 
 * 
 * Caller provides:
 *   state - game state struct containing all current info 
 *   message - message from server
 * Returns:
 *   nothing
 */
void handle_gold_message(client_game_state_t* state, char* message) 
{
  // n - amount collected in pile
  // p - amount in purse
  // r - gold remaining in game
  int n; 
  int p;
  int r;

  // read the message for GOLD n p r format
  sscanf(message, "GOLD %d %d %d", n, p, r);

  // update game state appropriately
  state->purseGold = p;
  state->goldRemaining = r;

  // update status line
  update_status_line(state);
}

/******************* update_status_line *****************/
/*
 * update_status_line - updates the status line to have the most current data from the server
 * 
 * Caller provides:
 *   state - game state struct containing all current info 
 * Returns:
 *   nothing
 */
void update_status_line(client_game_state_t* state) 
{
  // check to make sure status line is initialized
  if (state->statusLine == NULL) {
    fprintf(stderr, "Error: Status line is not initialized.\n");
    return;
  }

  if (state->purseGold >= 0 && state->goldRemaining >= 0) {
    if (!state->client->isSpectator) {
      // compose the status line for a player
      snprintf(state->statusLine, MAX_STATUS_LENGTH, 
      "Player %c has %d nuggets (%d nuggets unclaimed).",
      state->playerSymbol, state->purseGold, state->goldRemaining);
    }
    else {
      // compose the status line for a spectator
      snprintf(state->statusLine, MAX_STATUS_LENGTH,
      "Spectator: %d nugget unclaimed. Play at %s %d",
      state->goldRemaining, state->client->hostname, state->client->port);
    }
  }
}

/******************* handle_ok_message *****************/
/*
 * handle_ok_message - handles ok messages by storing the character as the player symbol
 * in the client game state struct
 * 
 * Caller provides:
 *   state - game state struct containing all current info
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "OK L", where 'L' 
 *   is the player's symbol
 * Returns:
 *   nothing
 */
void handle_ok_message(client_game_state_t* state, const char* message) 
{
  char playerSymbol;

  // parse the message for the player symbol
  if (sscanf(message, "OK %c", &playerSymbol) == 1) {
    // check to make sure it is a letter
    if (isalpha(playerSymbol)) {
      // store playersymbol in game state
      state->playerSymbol = playerSymbol;
    }
    else {
      fprintf(stderr, "Error: Invalid player symbol %c received from server\n");
      return;
    }
  }
}

/******************* handle_quit_message *****************/
/*
 * handle_quit_message - handles quit messages printing the provided explanantion
 * 
 * Caller provides:
 *   state - game state struct containing all current info
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "QUIT explanantion", where the explanation 
 *   is a string to be printed
 * Returns:
 *   nothing
 */
static void handle_quit_message(client_game_state_t* state, char* message)
{
  // buffer for holding the explanation from server 
  char explanation[MAX_QUIT_MESSAGE_LENGTH];

  // parse the message for the explanation
  if (sscanf(message, "QUIT %[^\n]", explanation) == 1) {
    // print the explanation
    printf(explanation);

    // cleanup time!
    free(state->display);
    free(state->statusLine);
    free(state->client);

    exit(0);  // exit with zero code to show success!
  }
  else {
    fprintf(stderr, "Error: Failed to parse QUIT message from server.\n");
  }
}

/******************* handle_error_message *****************/
/*
 * handle_error_message - handles error messages by updating the status line for the client
 * 
 * Caller provides:
 *   state - game state struct containing all current info
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "ERROR explanantion", where the explanation 
 *   is a string to be printed
 * Returns:
 *   nothing
 */
static void handle_error_message(client_game_state_t* client, char* message)
{
  
}