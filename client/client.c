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
#include "mem.h"

/**************** global types ****************/
// struct to hold necessary starting info for client to intitialize game
typedef struct client {
  addr_t server;
  char* statusLine;
  char playerSymbol;
  bool isSpectator;
  bool isQuitting;
} client_t;

/************* function prototypes ************/
void parseArgs(client_t* client, int argc, char* argv[]);
static void initializeDisplay(); 
static bool handleServerMessage(void* arg, const addr_t from, const char* message);
bool handleClientInput(void* arg);
void updateDisplay(char* gameState);
void displayStatusLine(const char* message);


/* helper functions for handleServerMessage */
void handleGridMessage(const char* message);
void checkDisplayDimensions(int num_rows, int num_cols);
void handleGoldMessage(client_t* client, const char* message); 
void handleOkMessage(client_t* client, const char* message); 
static void handleQuitMessage(const char* message);
static void handleErrorMessage(client_t* client, const char* message);
static void handleDisplayMessage(const char* message);
/***************************************************/


/******************* main ********************/
int main(int argc, char* argv[]) 
{
  FILE* log = fopen("client.log", "w");
  if (log == NULL) {
    fprintf(stderr, "Error opening log file\n");
    exit(1);
  }
  log_init(log); // initialize the log
  log_v("Client started.");

  // allocate memory for client
  client_t* client = mem_malloc(sizeof(client_t)); 
  if (client == NULL) { // check for memory allocation failure
    fprintf(stderr, "Error: Memory allocation failed\n");
    exit(1); 
  }

  // Initialize the allocated memory to zero
  memset(client, 0, sizeof(client_t));

  // allocate memory for statusLine
  client->statusLine = mem_malloc(message_MaxBytes);  // check for memory allocation failure
  if (client->statusLine == NULL) {
    fprintf(stderr, "Error: Memory allocation failed for statusLine\n");
    mem_free(client);  
    exit(1);
  }

  // initialize message module
  if (message_init(NULL) == 0) {
    mem_free(client->statusLine);
    mem_free(client);
    log_e("Initialization of message module failed");
    exit(1);
  }

  parseArgs(client, argc, argv);

  initscr(); // initialize ncurses screen
  log_v("ncurses screen initialized");

  initializeDisplay(); // initialize display
  log_v("Display initialized");

  log_v("Message loop started");
  bool ok = message_loop(client, 0, NULL, handleClientInput, handleServerMessage);
  log_v("Message loop ended");

  // clean up
  log_v("Cleaning up resources");
  message_done();
  log_done();
  if (client->statusLine != NULL) {
    mem_free(client->statusLine);
    client->statusLine = NULL;
  }  
  mem_free(client); 
  endwin();
  return ok? 0 : 1;
}

/******************* parseArgs *****************/
/* see client.h for description */
void parseArgs(client_t* client, int argc, char* argv[])
{
  // validate command line length
  if (argc < 3 || argc > 4) {
    log_e("Invalid command-line arguments");
    fprintf(stderr, "Usage: ./client hostname port [username] (username is optional)\n");
    exit(1); // invalid command line arguments
  }

  const char* hostname = argv[1];
  const char* port = argv[2];

  // if address valid
  if (!message_setAddr(hostname, port, &client->server)) {
    log_e("Error: couldn't create address");
    fprintf(stderr, "Error: Couldn't create address.\n");
    exit(1);
  }

  client->isQuitting = false;

  // determine if player or spectator
  if (argc == 4) { // fourth arg indicates player
    client->isSpectator = false;                         
    char playMessage[message_MaxBytes];
    sprintf(playMessage, "PLAY %s", argv[3]); // add the player's name
    message_send(client->server, playMessage);
    log_s("Message sent: %s", playMessage);
  }
  else { // else, is a spectator 
    client->isSpectator = true;
    char* spectateMessage = "SPECTATE";
    message_send(client->server, spectateMessage);
    log_s("Message sent: %s", spectateMessage);
  }
}

/******************* initializeDisplay *****************/
/* see client.h for description */
static void initializeDisplay() 
{
  cbreak(); // accept keystrokes without needing to hit enter 
  noecho(); // don't echo to the screen
}

/******************* handleServerMessage *****************/
/* see client.h for description */
static bool handleServerMessage(void* arg, const addr_t from, const char* message)
{
  client_t* client = (client_t*) arg;

  if (message == NULL) {
    log_e("Received NULL message from server");
    fprintf(stderr, "Received NULL message from server\n");
    return false;
  }

  log_s("Message received: %s", message);

  if (strncmp(message, "GRID ", strlen("GRID ")) == 0) {
    handleGridMessage(message);

  }
  else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0) {
    handleGoldMessage(client, message);
  }
  else if (strncmp(message, "OK ", strlen("OK ")) == 0) {
    handleOkMessage(client, message);
  }
  else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
    handleQuitMessage(message);
    return true; // stop looping
  }
  else if (strncmp(message, "ERROR", strlen("ERROR")) == 0) {
    handleErrorMessage(client, message);
  }
  else if (strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
    handleDisplayMessage(message);
  }
  else { // message not formatted correctly, log error
    mvprintw(0, 0, "Error: message from server could not be read.\n");
  }
  refresh();
  log_s("Message (%s) was handled correctly!", message);
  return false;
}

/******************* handleGridMessage *****************/
/* see client.h for description */
void handleGridMessage(const char* message)
{
  int rows; // variable to hold integer from server message
  int cols; // variable to hold integer from server message

  // update game state to have most curr rows and cols 
  if (sscanf(message, "GRID %d %d", &rows, &cols) == 2) {
    // check to make sure the dimensions of the display are valid
    checkDisplayDimensions(rows, cols);
  }
  else {
    fprintf(stderr, "Error: GRID message from server could not be read\n");
    return;
  }
}

/******************* checkDisplayDimensions *****************/
/* see client.h for description */
void checkDisplayDimensions(int num_rows, int num_cols) 
{
  int max_rows; // the number of rows the display could show
  int max_cols; // the number of columns the display could show

  // get curr display size
  getmaxyx(stdscr, max_rows, max_cols); 

  if (max_rows < (num_rows + 1) || max_cols < (num_cols + 1)) { // if current display is not large enough 
    mvprintw(0, 0, "Error: Screen size of display needs to be at least %d rows and %d columns", num_rows + 1, num_cols + 1);
    refresh();
    getmaxyx(stdscr, num_rows, num_cols); // get current display size again 
  }

  clear();
  return;
}

/******************* handleGoldMessage *****************/
/* see client.h for description */
void handleGoldMessage(client_t* client, const char* message) 
{
  // n - amount collected in pile
  // p - amount in purse
  // r - gold remaining in game
  int n, p, r;


  // read the message for GOLD n p r format
  if (sscanf(message, "GOLD %d %d %d", &n, &p, &r) == 3) {
    /************************** HELP!!!!!!!!!!!!!!!!!! */
    char* goldStatus = mem_malloc(message_MaxBytes);
    // write status line for spectator
    if (client->isSpectator) {
      sprintf(goldStatus, "Spectator: %d nuggets unclaimed.", r);
    }
    else {
      sprintf(goldStatus, "Player %c has %d nuggets (%d nuggets unclaimed).", client->playerSymbol, p, r);
    }
    
    strcpy(client->statusLine, goldStatus); // copy gold status into status line 

    // update status line if player collects gold
    char gold_collected[strlen("GOLD received: ") + 5];
    if (n > 0) {
      sprintf(gold_collected, "GOLD received: %d", n);
      char* totalStatus = mem_malloc(message_MaxBytes);
      sprintf(totalStatus, "%s %s", client->statusLine, gold_collected);
      displayStatusLine(totalStatus);
      refresh();
      mem_free(totalStatus);
    }
    else{
      displayStatusLine(client->statusLine);
    }

    
    refresh();
    mem_free(goldStatus);
  }
  else {
    fprintf(stderr, "Error: Gold message from server could not be read.\n");
  }

}

/******************* handleOkMessage *****************/
/* see client.h for description */
void handleOkMessage(client_t* client, const char* message) 
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

/******************* handleQuitMessage *****************/
/* see client.h for description */
// char* quitMessage
static void handleQuitMessage(const char* message)
{
  // end ncurses
  endwin();

  // make pointer to start of quit explanation
  // start at beginning of quit message from server 
  char* quitExplanation = strstr(message, "QUIT ");
  if (quitExplanation != NULL) {
    // skip "QUIT " by adding 5
    quitExplanation += 5;
    printf("%s\n", quitExplanation);
  }
}

/******************* handleErrorMessage *****************/
/* see client.h for description */
static void handleErrorMessage(client_t* client, const char* message)
{
  // make pointer to start of error explanation
  // start at beginning of error message from server 
  char* errorExplanation = strstr(message, "ERROR ");
  if (errorExplanation != NULL) {
    // skip "ERROR " by adding 6
    errorExplanation += 6;
  }

  char* totalStatus = mem_malloc(message_MaxBytes);

  sprintf(totalStatus, "%s %s", client->statusLine, errorExplanation);

  displayStatusLine(totalStatus);
  refresh();
  mem_free(totalStatus);
}

/******************* handleDisplayMessage *****************/
/* see client.h for description */
static void handleDisplayMessage(const char* message)
{
  // skip the "DISPLAY\n" portion of the message to retrieve the display itself
  const char* displayMessage = message + strlen("DISPLAY\n");

  // copy the game state
  char* gameState = mem_malloc(strlen(displayMessage) + 1);
  if (gameState == NULL) {
    fprintf(stderr, "Error: Memory allocation failed for gameState\n");
    return;
  }
  strcpy(gameState, displayMessage);

  updateDisplay(gameState);

  // mem_free(displayMessage);
  mem_free(gameState);
}

/******************* handleClientInput *****************/
/* see client.h for description */
bool handleClientInput(void* arg) 
{
  char inputCharacter; // int to hold client keystroke
  char message[message_MaxBytes]; // buffer for holding client input
  // cast arg to client struct pointer
  client_t* client = (client_t*) arg;

  if (!client->isSpectator) {
    if (!message_isAddr(client->server)) {
      fprintf(stderr, "handleInput called without a correspondent.\n");
      return true;
    }
    
    if (client->isQuitting) {
      return true; // end loop
    }

    // read one character from stdin
    inputCharacter = getch();
    if (inputCharacter == EOF) { // check to make sure not EOF
      message_send(client->server, "KEY Q");
      return true; // if it is stop looping
    } 

    if (inputCharacter == 'Q') { // mark that the client is trying to quit
      client->isQuitting = true; // but don't return true to allow the loop one last loop
    }

    // create the message to server
    snprintf(message, sizeof(message), "KEY %c", inputCharacter);

    message_send(client->server, message);
  }

  return false; // keep the message loo;p going
}

/******************* displayStatusLine *****************/
/* see client.h for description */
void displayStatusLine(const char* message)
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