# CS50 Nuggets
## CS50 Fall 2024

### Client for the Nuggets Game

This module implements the client-side functionality for the Nuggets game.  The client connects to a server as either a player or spectator.  The user's keystrokes are taken and sent to the server as KEY messages and server messages are processed to update the display of the client.

### Known Errors (Cleared with Professor Palmer)

* Sometimes, the message loop reads twice upon game ending randomly.  I talked to Professor Palmer about this in class, and we could not discern why this happened.

* Sometimes, the screen upon initialization of the client doesn't work the first time client is run, but if you exit, then rerun client it works, again also with no rhyme or reason.

* Ncurses leaks.