/* 
 * client.h - header file for client module in the Nuggets game
 *
 * client.c handles the client side implementation of the nuggets game. 
 * It initializes the client as either player or spectator, if they are a player
 * stdin is collected and sent to the server and then the server sends back a message containing relavant
 * info to update the client's screen.  
 * 
 * Caroline Chung - November 12, 2024
 */

#ifndef __CLIENT_H
#define __CLIENT_H

#include <stdio.h>
#include <stdbool.h>
#include "message.h" // For addr_t

/**************** global types ****************/

// Struct to hold necessary starting info for client initialization
typedef struct client_init client_init_t;

// Struct to hold current game state information
typedef struct client_game_state client_game_state_t;

/**************** functions ****************/

/******************* initialize_client *****************/
/*
 * initialize_client - inititalizes the connection to server, calls parse args to populate 
 * client information, then sends either 'PLAY' or 
 * 'SPECTATE' message to server.
 *
 * Caller provides:
 *   state - pointer to client_game_state_t struct to store game state info  
 *   serverAddress - address of the server
 *   logFP - location of where relevant output goes 
 *   argc - the number of command line args
 *   argv - array of command line args 
 * Returns:
 *   True if initialization works.  False otherwise.
 */
bool initialize_client(client_game_state_t* state, addr_t* serverAddress, FILE* logFP, int argc, char* argv[]);

/******************* parseArgs *****************/
/*
 * parseArgs - Validates command line arguments.
 *
 * Caller provides:
 *   client - a pointer to a client_init_t struct that holds all info needed about 
 *            client to initialize game
 *   argc - number of command line args
 *   argv - array of command line args
 *     argv[1] - hostname
 *     argv[2] - port where the client will connect to server
 *     argv[3] - playername (optional)
 * Returns:
 *   nothing
 */
void parseArgs(client_init_t* client, int argc, char* argv[]);

/******************* initialize_display *****************/
/*
 * initialize_display - sets up the display initially and checks for 
 * valid window size based on "GRID" message from server.
 *
 * Caller provides:
 *   state - pointer to client_game_state_t struct to store game state info  
 *   num_rows - number of rows for the display
 *   num_cols - number of columns for the display
 * Returns:
 *   True if initialization of display works.  False otherwise.
 */
bool initialize_display(client_game_state_t* state, int num_rows, int num_cols);

/******************* handle_server_message *****************/
/*
 * handle_server_message - handles server messages, calling relevant
 * helper functions as necessary
 * 
 * Caller provides:
 *   arg - pointer to client_game_state_t struct containing current game state
 *   from - address of the server 
 *   message - message received from the server
 * Notes:
 *   Server messages are expected to start with "OK", "GRID", "DISPLAY",
 *   "GOLD", "QUIT", or "ERROR"
 * Returns:
 *   True if message handling works.  False otherwise.
 */
static bool handle_server_message(void* arg, const addr_t from, const char* message);

/******************* handle_grid_message *****************/
/*
 * handle_grid_message - handles "GRID" server messages, updating client game state
 * and checking that the display is sufficiently large using helper function called
 * check_display_dimensions.
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state
 *   message - message from server
 * Returns:
 *   nothing
 */
void handle_grid_message(client_game_state_t* state, const char* message);

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
static bool check_display_dimensions(int num_rows, int num_cols);

/******************* handle_gold_message *****************/
/*
 * handle_gold_message - reads the relevant message from server, 
 * parsing message for n (amount of gold collected in pile), p 
 * (amount of gold in purse), and r (amount of gold remaining) 
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state
 *   message - message from server
 * Returns:
 *   nothing
 */
void handle_gold_message(client_game_state_t* state, const char* message);

/******************* update_status_line *****************/
/*
 * update_status_line - updates the status line to have the most current data from the server
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state 
 * Returns:
 *   nothing
 */
void update_status_line(client_game_state_t* state);

/******************* handle_ok_message *****************/
/*
 * handle_ok_message - handles ok messages by storing the character as the player symbol
 * in the client game state struct
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state 
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "OK L", where 'L' 
 *   is the player's symbol
 * Returns:
 *   nothing
 */
void handle_ok_message(client_game_state_t* state, const char* message);

/******************* handle_quit_message *****************/
/*
 * handle_quit_message - handles quit messages printing the provided explanantion
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state 
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "QUIT explanantion", where the explanation 
 *   is a string to be printed
 * Returns:
 *   nothing
 */
static void handle_quit_message(client_game_state_t* state, const char* message);

/******************* handle_error_message *****************/
/*
 * handle_error_message - handles error messages by updating the status line for the client
 * with the explanantion parsed from the server message
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state 
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "ERROR explanantion", where the explanation 
 *   is a string to be added to the status line.
 * Returns:
 *   nothing
 */
static void handle_error_message(client_game_state_t* state, const char* message);

/******************* handle_display_message *****************/
/*
 * handle_display_message - handles DISPLAY messages by printing the display string provided by 
 * the server
 * 
 * Caller provides:
 *   state - pointer to client_game_state_t struct containing current game state 
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "DISPLAY explanation", where the explanation 
 *   is a string to be printed so the client can see on their end.
 * Returns:
 *   nothing
 */
static void handle_display_message(client_game_state_t* state, const char* message);

/******************* handle_client_input *****************/
/*
 * handle_client_input - handles client input, sends message thats appropriately
 * formatted to the server
 * 
 * Caller provides:
 *   arg - a pointer to the server's addr_t address 
 * Notes:
 *   messages to server should be formatted as "KEY k" where k in the client input
 *   also based on handleInput function in miniclient.c
 * Returns:
 *   True if the loop should exit on EOF or fatal error, false otherwise.
 */
static bool handle_client_input(void* arg);

#endif // __CLIENT_H
