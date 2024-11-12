/* 
 * client.c - a client using the messaging module
 *
 * client.c handles the client side implementation of the nuggets game. 
 * It initializes the client as either player or spectator, if they are a player
 * stdin is collected and sent to the server and then the server sends back a message containing relavant
 * info to update the client's screen.  
 * 
 * Caroline Chung - November 12, 2024
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include "ctype.h"
#include "message.h"
#include "log.h"

#define MAX_NAME_LENGTH 50 // max number of chars in playerName

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
bool initialize_client(client_game_state_t* state, addr_t* serverAddress, FILE* logFP, int argc, char* argv[]);
void parseArgs(client_init_t* client, int argc, char* argv[]);
bool initialize_display(client_game_state_t* state, int num_rows, int num_cols); 
static bool handle_server_message(void* arg, const addr_t from, const char* message);
static bool handle_client_input(void* arg);

/* moduler helper functions called by handle_server_message */
void handle_grid_message(client_game_state_t* state, const char* message);
static bool check_display_dimensions(int num_rows, int num_cols); 
void handle_gold_message(client_game_state_t* state, const char* message);
void handle_gold_message(client_game_state_t* state, const char* message);
void update_status_line(client_game_state_t* state);
void handle_ok_message(client_game_state_t* state, const char* message);
static void handle_quit_message(client_game_state_t* state, const char* message);
static void handle_error_message(client_game_state_t* state, const char* message);
static void handle_display_message(client_game_state_t* state, const char* message);
/************************************************** */

/******************* main ********************/
int main(int argc, char* argv[]) 
{
  FILE* logFP = stderr; // log file for error messages

  // initialize game state
  client_game_state_t state;
  state.client = malloc(sizeof(client_init_t));
  if (state.client == NULL) {
    fprintf(stderr, "Error: Unable to allocate memory for client initialization structure");
    return 1;
  }

  // initialize client
  addr_t serverAddress;
  if (!initialize_client(&state, &serverAddress, logFP, argc, argv)) {
    fprintf(stderr, "Error: Client initialization failed\n");
    free(state.client);
    return 1;
  }

  // start ncurses for display 
  initscr();  // Start ncurses mode
  cbreak();   // Disable line buffering
  noecho();   // Don't echo user input
  keypad(stdscr, TRUE);  // Enable special keys

  // start the message loop
  bool ok = message_loop(&serverAddress, 0, NULL, handle_client_input, handle_server_message);

  // finish ncurses
  endwin();
  message_done();
  log_done();

  // clean up dyanmically allocated memory
  free(state.client);
  free(state.display);
  free(state.statusLine);

  return ok ? 0 : 1;  // return based on result of message loop
}

/******************* initialize_client *****************/
/* see client.h for description */
bool initialize_client(client_game_state_t* state, addr_t* serverAddress, FILE* logFP, int argc, char* argv[])
{
  // initialize logging module
  log_init(logFP); 

  if (message_init(logFP) == 0) { // check for failure to initialize
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
  state->statusLine = malloc(message_MaxBytes * sizeof(char));
  if (state->statusLine == NULL) {
    fprintf(stderr, "Error: Unable to allocate memory for status line.\n");
    return false;
  }
  
  // Send the initial message based on whether the client is a player or a spectator
  if (state->client->isSpectator) {
    message_send(*serverAddress, "SPECTATE");
  } else {
    char playMessage[message_MaxBytes];
    snprintf(playMessage, sizeof(playMessage), "PLAY %s", state->client->playerName);
    message_send(*serverAddress, playMessage);
  }

  return true;
}

/******************* parseArgs *****************/
/* see client.h for description */
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
  if (atoi(argv[2]) > 0) {
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
/* see client.h for description */
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
/* see client.h for description */
static bool handle_server_message(void* arg, const addr_t from, const char* message)
{
  client_game_state_t* state = arg;

  if (strncmp(message, "GRID", 4) == 0) {
    handle_grid_message(state, message);
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
  else if (strncmp(message, "ERROR", 5) == 0) {
    handle_error_message(state, message);
  }
  else if (strncmp(message, "DISPLAY", 7) == 0) {
    handle_display_message(state, message);
  }
  else {
    return false;
  }
  
  return true;
}

/******************* handle_grid_message *****************/
/* see client.h for description */
void handle_grid_message(client_game_state_t* state, const char* message)
{
  int rows;
  int cols; 

  // update game state to have most curr rows and cols 
  if (sscanf(message, "GRID %d %d", &rows, &cols) == 1) {
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
  else {
    fprintf(stderr, "Error: GRID message from server could not be read\n");
  }
}

/******************* check_display_dimensions *****************/
/* see client.h for description */
static bool check_display_dimensions(int num_rows, int num_cols) 
{
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
/* see client.h for description */
void handle_gold_message(client_game_state_t* state, const char* message) 
{
  // n - amount collected in pile
  // p - amount in purse
  // r - gold remaining in game
  int n; 
  int p;
  int r;

  // read the message for GOLD n p r format
  if (sscanf(message, "GOLD %d %d %d", &n, &p, &r) == 1) {
    // update game state appropriately
    state->purseGold = p;
    state->goldRemaining = r;

    // update status line
    update_status_line(state);
  }
  else {
    fprintf(stderr, "Error: Gold message from server could not be read.\n");
  }

}

/******************* update_status_line *****************/
/* see client.h for description */
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
      snprintf(state->statusLine, message_MaxBytes, 
      "Player %c has %d nuggets (%d nuggets unclaimed).",
      state->playerSymbol, state->purseGold, state->goldRemaining);
    }
    else {
      // compose the status line for a spectator
      snprintf(state->statusLine, message_MaxBytes,
      "Spectator: %d nugget unclaimed. Play at %s %d",
      state->goldRemaining, state->client->hostname, state->client->port);
    }
  }
}

/******************* handle_ok_message *****************/
/* see client.h for description */
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
      fprintf(stderr, "Error: Invalid player symbol %c received from server\n", playerSymbol);
      return;
    }
  }
}

/******************* handle_quit_message *****************/
/* see client.h for description */
static void handle_quit_message(client_game_state_t* state, const char* message)
{
  // buffer for holding the explanation from server 
  char explanation[message_MaxBytes];

  // parse the message for the explanation
  if (sscanf(message, "QUIT %[^\n]", explanation) == 1) {
    // print the explanation
    printf("%s", explanation);

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
/* see client.h for description */
static void handle_error_message(client_game_state_t* state, const char* message)
{
  // buffer for holding the explanation from server 
  char explanation[message_MaxBytes];

  // parse the message for the explanation
  if (sscanf(message, "ERROR %[^\n]", explanation) == 1) {
    // print the explanation
    snprintf(state->statusLine, message_MaxBytes, "Error: %s", explanation);
  }
  else {
    fprintf(stderr, "Error: Failed to parse ERROR message from server.\n");
  }
}

/******************* handle_display_message *****************/
/* see client.h for description */
static void handle_display_message(client_game_state_t* state, const char* message)
{
  // Parse the message to extract the display content after "DISPLAY "
  if (sscanf(message, "DISPLAY %[^\n]", state->display) == 1) {
    clear(); // clear the outdated display 
    mvprintw(1, 0, "%s", state->statusLine); 
    mvprintw(2, 0, "%s", state->display);
    refresh();  // refresh the screen to throw up the most current display
  }
  else {
    fprintf(stderr, "Error: Failed to parse DISPLAY message from server\n");
  }
}

/******************* handle_client_input *****************/
/* see client.h for description */
static bool handle_client_input(void* arg) 
{
  int inputCharacter; // int to hold client keystroke
  char message[message_MaxBytes]; // buffer for holding client input
  // use 'arg' to get the server address
  addr_t* serverp = arg;
  if (serverp == NULL) {  // defensive checks to ensure serverAddress is valid
    fprintf(stderr, "handleInput called with arg=NULL");
    return true;
  }
  if (!message_isAddr(*serverp)) {
    fprintf(stderr, "handleInput called without a correspondent.");
    return true;
  }

  // read one character from stdin
  inputCharacter = getchar();
  if (inputCharacter == EOF) { // check to make sure not EOF
    return true; // if it is stop looping
  } 

  // create the message to server
  snprintf(message, sizeof(message), "KEY %c", inputCharacter);

  // send off the message!
  message_send(*serverp, message);

  return false; // keep the message loop going
}
