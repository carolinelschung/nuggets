/* 
 * client.h - header file for client module in the Nuggets game
 *
 * client handles the client side implementation of the nuggets game. 
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
#include <string.h>
#include <stdlib.h>
#include <curses.h>
#include "ctype.h"
#include "log.h"
#include "mem.h"
#include "message.h"

/**************** global types ****************/

// Struct to hold necessary starting info for client initialization
typedef struct client client_t;
/**************** functions ****************/

/******************* parseArgs *****************/
/*
 * parseArgs - Validates command line arguments.
 *
 * Caller provides:
 *   client - a pointer to a client_t struct that holds all info needed about 
 *            client 
 *   argc - number of command line args
 *   argv - array of command line args
 *     argv[1] - hostname
 *     argv[2] - port where the client will connect to server
 *     argv[3] - playername (optional)
 * Returns:
 *   nothing
 */
void parseArgs(client_t* client, int argc, char* argv[]);

/******************* initializeDisplay *****************/
/*
 * initializeDisplay - starts ncurses
 *
 * Caller provides:
 *   Nothing
 * Returns:
 *   Nothing
 */
static void initializeDisplay(); 

/******************* handleServerMessage *****************/
/*
 * handleServerMessage - handles server messages, calling relevant
 * helper functions as necessary
 * 
 * Caller provides:
 *   arg - pointer to client_t struct containing client info
 *   from - address of the server 
 *   message - message received from the server
 * Notes:
 *   Server messages are expected to start with "OK", "GRID", "DISPLAY",
 *   "GOLD", "QUIT", or "ERROR"
 * Returns:
 *   True to stop message_loop.  False to keep message_loop going.
 */
static bool handleServerMessage(void* arg, const addr_t from, const char* message);

/******************* handleGridMessage *****************/
/*
 * handleGridMessage - handles "GRID" server messages, updating client game state
 * and checking that the display is sufficiently large.
 * 
 * Caller provides:
 *   client - pointer to client_t struct 
 *   message - message from server
 * Notes:
 *   message from server should be formatted as "GRID rows cols".
 * Returns:
 *   nothing
 */
void handleGridMessage(client_t* client, const char* message);

/******************* handleGoldMessage *****************/
/*
 * handleGoldMessage - reads the relevant message from server, 
 * parsing message for n (amount of gold collected in pile), p 
 * (amount of gold in purse), and r (amount of gold remaining) 
 * 
 * Caller provides:
 *   client - pointer to client_t struct
 *   message - message from server
 * Notes:
 *   message from server should be formatted as "GOLD n p r", where the explanation 
 *   is a string to be printed so the client can see on their end.
 * Returns:
 *   nothing
 */
void handleGoldMessage(client_t* client, const char* message); 

/******************* displayStatusLine *****************/
/*
 * displayStatusLine - updates the status line on screen for the user 
 * 
 * Caller provides:
 *   message - message to be displayed in the status line 
 * Returns:
 *   nothing
 */
void displayStatusLine(const char* message);

/******************* updateDisplay *****************/
/*
 * updateDisplay - updates the display on screen for the user 
 * 
 * Caller provides:
 *   gameState - the display with the map to be displayed on screen for the player
 * Returns:
 *   nothing
 */
void updateDisplay(char* gameState);

/******************* handleOkMessage *****************/
/*
 * handleOkMessage - handles ok messages by storing the character as the player symbol
 * in the client game state struct
 * 
 * Caller provides:
 *   client - pointer to client_t struct containing current game state 
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "OK L", where 'L' 
 *   is the player's symbol
 * Returns:
 *   nothing
 */
void handleOkMessage(client_t* client, const char* message); 

/******************* handleQuitMessage *****************/
/*
 * handleQuitMessage - handles quit messages printing the provided explanantion
 * 
 * Caller provides:
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "QUIT explanantion", where the explanation 
 *   is a string to be printed
 * Returns:
 *   nothing
 */
static void handleQuitMessage(const char* message);

/******************* handleErrorMessage *****************/
/*
 * handleErrorMessage - handles error messages by updating the status line for the client
 * with the explanantion parsed from the server message
 * 
 * Caller provides:
 *   client - pointer to client_t struct containing current game state 
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "ERROR explanantion", where the explanation 
 *   is a string to be added to the status line.
 * Returns:
 *   nothing
 */
static void handleErrorMessage(client_t* client, const char* message);

/******************* handle_display_message *****************/
/*
 * handle_display_message - handles DISPLAY messages by printing the display string provided by 
 * the server
 * 
 * Caller provides:
 *   message - message from server 
 * Notes:
 *   message from server should be formatted as "DISPLAY explanation", where the explanation 
 *   is a string to be printed so the client can see on their end.
 * Returns:
 *   nothing
 */
static void handleDisplayMessage(const char* message);

/******************* handleClientInput *****************/
/*
 * handleClientInput - handles client input, sends message thats appropriately
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
bool handleClientInput(void* arg);

#endif // __CLIENT_H
