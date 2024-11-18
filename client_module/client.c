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
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ctype.h"
#include "message.h"
#include "log.h"

/**************** global types ****************/
// struct to hold necessary starting info for client to intitialize game
typedef struct client {
  addr_t server;
  char* statusLine;
  char playerSymbol;
  bool isSpectator;
  bool quitting; 
  char quitMessage[7000];
} client_t;

/************* function prototypes ************/
void parseArgs(client_t* client, int argc, char* argv[]);
static void initialize_display(); 
static bool handle_server_message(void* arg, const addr_t from, const char* message);
void handle_grid_message(const char* message);
void check_display_dimensions(int num_rows, int num_cols);
void handle_gold_message(client_t* client, const char* message); 
void handle_ok_message(client_t* client, const char* message); 
static void handle_quit_message(const char* message);
static void handle_error_message(client_t* client, const char* message);
static void handle_display_message(const char* message);
bool handle_client_input(void* arg);
void displayStatus(const char* message);
void updateDisplay(char* gameState);
/***************************************************/


/******************* main ********************/
int main(int argc, char* argv[]) 
{
  client_t* client = malloc(sizeof(client_t));
  if (client == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1);
  }
  // Initialize the allocated memory to zero
  memset(client, 0, sizeof(client_t));

  client->statusLine = malloc(message_MaxBytes);
  if (client->statusLine == NULL) {
    fprintf(stderr, "Error: Memory allocation failed for statusLine\n");
    free(client);
    exit(1);
  }

  if (message_init(NULL) == 0) {
    exit(1);
  }

  parseArgs(client, argc, argv);

  initscr(); // initialize ncurses screen

  initialize_display(); // initialize display

  log_init(stderr); // initialize the log

  bool ok = message_loop(client, 0, NULL, handle_client_input, handle_server_message);

  message_done();
  // log_done();
  free(client->statusLine);
  free(client);
  // endwin(); // stop ncurses

  return ok? 0 : 1;
}

/******************* parseArgs *****************/
/* see client.h for description */
void parseArgs(client_t* client, int argc, char* argv[])
{
  // validate command line length
  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Usage: ./client hostname port <unique username> (username is optional)\n");
    exit(1); // invalid command line arguments
  }

  const char* hostname = argv[1];
  const char* port = argv[2];

  // const addr_t server = tempServer;
  client->quitting = false;

  // if address valid
  if (!message_setAddr(hostname, port, &client->server)) {
    fprintf(stderr, "Error: Couldn't create address.\n");
    exit(1);
  }

  // determine if player or spectator
  if (argc == 4) { // player
    client->isSpectator = false; // fourth arg indicates player                        
    char message[message_MaxBytes];
    sprintf(message, "PLAY ");
    strcat(message, argv[3]);
    message_send(client->server, message);
  }
  else {
    client->isSpectator = true;
    char* message = "SPECTATE";
    message_send(client->server, message);
  }
}

/******************* initialize_display *****************/
/* see client.h for description */
static void initialize_display() 
{
  cbreak(); // accept keystrokes without needing to hit enter 
  noecho(); // don't echo to the screen
}

/******************* handle_server_message *****************/
/* see client.h for description */
static bool handle_server_message(void* arg, const addr_t from, const char* message)
{
  client_t* client = (client_t*) arg;

  if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
    const char* me = "cc";
    log_s("HANDLING QUIT %s", me);
    // fprintf("handling\n");
    // fflush(stdout);
    handle_quit_message(message);
    return true; // stop looping
  }

  if (message == NULL) {
    fprintf(stderr, "Received NULL message from server\n");
    return false;
  }


  if (strncmp(message, "GRID ", strlen("GRID ")) == 0) {
    handle_grid_message(message);
  }
  else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0) {
    handle_gold_message(client, message);
  }
  else if (strncmp(message, "OK ", strlen("OK ")) == 0) {
    handle_ok_message(client, message);
  }
  // else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
  //   handle_quit_message(message);
  //   return true; // stop looping
  // }
  else if (strncmp(message, "ERROR", strlen("ERROR")) == 0) {
    handle_error_message(client, message);
  }
  else if (strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
    handle_display_message(message);
  }
  else { // message not formatted correctly, log error
    mvprintw(0, 0, "Error: %s \n", message);
  }
  refresh();
  return false;
}

/******************* handle_grid_message *****************/
/* see client.h for description */
void handle_grid_message(const char* message)
{
  int rows;
  int cols; 

  // update game state to have most curr rows and cols 
  if (sscanf(message, "GRID %d %d", &rows, &cols) == 2) {
    // check to make sure the dimensions of the display are valid
    check_display_dimensions(rows, cols);
  }
  else {
    fprintf(stderr, "Error: GRID message from server could not be read\n");
    return;
  }
}

/******************* check_display_dimensions *****************/
/* see client.h for description */
void check_display_dimensions(int num_rows, int num_cols) 
{
  int max_rows; // the number of rows the display could show
  int max_cols; // the number of columns the display could show

  // get curr display size
  getmaxyx(stdscr, max_rows, max_cols); 

  if (max_rows < (num_rows + 1) || max_cols < (num_cols + 1)) {
    mvprintw(0, 0, "Error: Screen size of display needs to be at least %d rows and %d columns", num_rows + 1, num_cols + 1);
    refresh();
    getmaxyx(stdscr, num_rows, num_cols);
  }

  clear();
  return;
}

/******************* handle_gold_message *****************/
/* see client.h for description */
void handle_gold_message(client_t* client, const char* message) 
{
  // n - amount collected in pile
  // p - amount in purse
  // r - gold remaining in game
  int n, p, r;

  // read the message for GOLD n p r format
  if (sscanf(message, "GOLD %d %d %d", &n, &p, &r) == 3) {
    /************************** HELP!!!!!!!!!!!!!!!!!! */
    char* goldStatus = malloc(sizeof(char) * message_MaxBytes);
    // write status line for spectator
    if (client->isSpectator) {
      sprintf(goldStatus, "Spectator: %d nuggets unclaimed.", r);
    }
    else {
      sprintf(goldStatus, "Player %c has %d nuggets (%d nuggets unclaimed).", client->playerSymbol, p, r);
    }

    client->statusLine = goldStatus;

    // update status line if player collects gold
    char gold_collected[strlen("GOLD received: ") + 5];
    if (n > 0) {
      sprintf(gold_collected, "GOLD received: %d", n);
      char* totalStatus = malloc(sizeof(char) * (strlen(goldStatus) + strlen(gold_collected) + 1));
      sprintf(totalStatus, "%s %s", client->statusLine, gold_collected);
      displayStatus(totalStatus);
    }
    else{
      displayStatus(client->statusLine);
    }

    refresh();
  }
  else {
    fprintf(stderr, "Error: Gold message from server could not be read.\n");
  }

}

/******************* handle_ok_message *****************/
/* see client.h for description */
void handle_ok_message(client_t* client, const char* message) 
{
  char playerSymbol;

  // parse the message for the player symbol
  if (sscanf(message, "OK %c", &playerSymbol) == 1) {
    // check to make sure it is a letter
    if (isalpha(playerSymbol)) {
      // store playersymbol in game state
      client->playerSymbol = playerSymbol;
    }
    else {
      fprintf(stderr, "Error: Invalid player symbol %c received from server\n", playerSymbol);
      return;
    }
  }
}

/******************* handle_quit_message *****************/
/* see client.h for description */
// char* quitMessage
static void handle_quit_message(const char* message)
{
  // printf("In quit message");
  // end ncurses
  endwin();

  // make pointer to start of quit explanation
  // start at beginning of quit message from server 
  char* quitExplanation = strstr(message, "QUIT ");
  if (quitExplanation != NULL) {
    // skip "QUIT " by adding 5
    quitExplanation += 5;
    printf("%s\n", quitExplanation);
    // printf("%ld", strlen(quitExplanation));
  }
  // *(client->quitMessage) = '\0';
  // if (client->quitMessage != NULL) {
  //   strcpy(client->quitMessage, message + strlen("QUIT "));
  // }
}

/******************* handle_error_message *****************/
/* see client.h for description */
static void handle_error_message(client_t* client, const char* message)
{
  // make pointer to start of error explanation
  // start at beginning of error message from server 
  char* errorExplanation = strstr(message, "ERROR ");
  if (errorExplanation != NULL) {
    // skip "ERROR " by adding 6
    errorExplanation += 6;
  }

  char* totalStatus = malloc(sizeof(char) * (strlen(errorExplanation) + strlen(client->statusLine) + 1));

  sprintf(totalStatus, "%s %s", client->statusLine, errorExplanation);
  // strncat(client->statusLine, errorExplanation, message_MaxBytes);

  displayStatus(totalStatus);
  refresh();
}

/******************* handle_display_message *****************/
/* see client.h for description */
static void handle_display_message(const char* message)
{
  // skip the "DISPLAY\n" portion of the message to retrieve the display itself
  const char* displayMessage = message + strlen("DISPLAY\n");

  // copy the game state
  char* gameState = malloc(strlen(displayMessage) + 1);
  if (gameState == NULL) {
    fprintf(stderr, "Error: Memory allocation failed for gameState\n");
    return;
  }
  strcpy(gameState, displayMessage);

  updateDisplay(gameState);

  free(gameState);
}

/******************* handle_client_input *****************/
/* see client.h for description */
bool handle_client_input(void* arg) 
{
  int inputCharacter; // int to hold client keystroke
  char message[message_MaxBytes]; // buffer for holding client input
  // cast arg to client struct pointer
  client_t* client = (client_t*) arg;

  if (!client->isSpectator) {
    if (!message_isAddr(client->server)) {
      fprintf(stderr, "handleInput called without a correspondent.\n");
      return true;
    }
    
    // if (client->quitting) {
    //   // endwin();
    //   return true;
    // }

    // read one character from stdin
    inputCharacter = getch();
    if (inputCharacter == EOF) { // check to make sure not EOF
      message_send(client->server, "KEY Q");
      return true; // if it is stop looping
    } 

    // if (inputCharacter == 'Q') { 
    //   client->quitting = true;              
    // }

    // create the message to server
    snprintf(message, sizeof(message), "KEY %c", inputCharacter);

    message_send(client->server, message);
  }

  return false; // keep the message loo;p going
}

/******************* displayStatus *****************/
/* see client.h for description */
void displayStatus(const char* message)
{
  if (message != NULL) {
    mvprintw(0, 0, "%s", message);
  }

  clrtoeol(); // erase current line from cursor to eol
}

/******************* updateDisplay *****************/
/* see client.h for description */
void updateDisplay(char* gameState)
{ 
  if (gameState == NULL) {
    fprintf(stderr, "Error: NULL gameState in updateDisplay\n");
    return;
  }

  // clear the current screen
  for (int row = 1; row < LINES; row++) { // for every line below the status line
    move(row, 0); // go the the start of the line with the cursor
    clrtoeol(); // clear it
  }

  // print the display 
  mvprintw(1, 0, "%s", gameState);
  refresh();
}