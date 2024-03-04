/*
 * server.c
 *
 * Launches the server for the Nuggets game. 
 * The server manages all messaging and game logic to all the clients.
 *
 * Usage: server map.txt [seed]
 *
 * Author: TEAM TORPEDOS - Sam Starrs, March 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#include "game.h"
#include "player.h"
#include "message.h"
#include "set.h"
#include "grid.h"
#include "mem.h"

/**************** local functions ****************/
/* not visible outside this file */
static void parseArgs(const int argc, char* argv[],
                      FILE** map, int* seed);
static bool handleMessage(void* arg, const addr_t from, const char* buf);
static void handlePlay(void* arg, const addr_t from, const char* content);
static bool  handleKey(void* arg, const addr_t from, const char* content);
static void handleSpectate(void* arg, const addr_t from, const char* content);
static void keyQ(const addr_t from);
static void errorMessage(const addr_t from, const char* content);
static bool checkWhitespace(const char* name);
static char* fixName(const char* entry);

/****** global variables *******/
const int MAXNAMELENGTH = 50;   // max number of chars in playerName
const int MAXPLAYERS = 26;      // maximum number of players

game_t* game;

/**************** main() ****************/
int main(const int argc, char* argv[]) {

    // Create variables
    FILE* map = NULL;
    int seed;

    // Parse arguments, open map and set seed
    parseArgs(argc, argv, &map, &seed);

    // Intialize game
    game = game_init(map);

    // Close map file
    fclose(map);

    // Open the server up for messages
    int port = message_init(stderr);
    if (port == 0) {
        return 2;
    } else {
        printf("serverPort=%d\n", port);
    }

    // Loop through messages and return 0 or 1
    bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);
    message_done();
    return ok? 0 : 1;

}

/* ******************* parseArgs ******************* */
/* Handle the messy business of parsing command-line arguments.
 * 
 * Adapted from parseArgs.c on CS50 Github
 */
static void parseArgs(const int argc, char* argv[],
                      FILE** map, int* seed) {

    // Should only be 2 or 3 arguments
    if (argc == 2 || argc == 3){

        char* mapString = argv[1];

        if ((*map = fopen(mapString, "r")) == NULL) {
            fprintf(stderr, "ERROR: Couldn't read file at '%s'\n", mapString);
            exit(2);
        }

        // If there is indeed a 3rd argument, set up seed
        if (argc == 3) {
            char* seedString = argv[2];
            // Try to convert seedString to an int
            *seed = 0; // initialize calling function's value
            char excess; // any excess chars after the number
    
            // If the argument is valid, sscanf should extract one int.
            if (sscanf(seedString, "%d%c", seed, &excess) != 1) {
                fprintf(stderr, "ERROR: '%s' invalid integer\n", seedString);
                exit(4);
            }

            // Check range of seed
            if (*seed < 0) {
                fprintf(stderr, "ERROR: '%d' must be > 0\n", *seed);
                exit(5);
            } else {
                srand(*seed);
            }
        } else {
            srand(getpid());
        }

    // If there are an unacceptable number of arguments
    } else {
        fprintf(stderr, "USAGE: server map.txt [seed]\n");
        exit(1);
    }

}

/**************** handleMessage() ****************/
/* Takes an optional pointer, address of the player, and string. 
 * Called in the message_loop() function.
 * Handles messaging from client.
 * Returns true/false based on if the game is over.
 */
static bool handleMessage(void* arg, const addr_t from, const char* message) {

    bool gameOver = false;

    // PLAY message - SYNTAX: PLAY real name
    if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
        const char* content = message + strlen("PLAY ");
        handlePlay(arg, from, content);
    // KEY message - SYNTAX: KEY k
    } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
        const char* content = message + strlen("KEY ");
        gameOver = handleKey(arg, from, content);
    // SPECTATE message - SYNTAX: SPECTATE
    } else if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0) {
        const char* content = message + strlen("SPECTATE");
        handleSpectate(arg, from, content);
    } 

    return gameOver;
}

/**************** handlePlay() ****************/
/* Takes an optional pointer, address of the player, and string. 
 * Called in the handlePlay() function.
 * Handles message for when client says PLAY
 */
static void handlePlay(void* arg, const addr_t from, const char* content) {
    
    // Make empty string isn't passed as name
    if(!checkWhitespace(content)) {
        // Check if game is full
        if (game_numPlayers(game) != MAXPLAYERS) {

            // Intialize and add player
            int x;
            int y;
            grid_findRandomSpawnPosition(game_masterGrid(game), &x, &y);
            char playerLetter = 'A' + game_numPlayers(game);
            char* name = fixName(content);
            player_t* player = player_new(from, x, y, name, playerLetter);
            game_addPlayer(game, player);

            // Send OK message
            char* okMessage = mem_malloc((sizeof(char) * strlen("OK A")) + 1);
            sprintf(okMessage, "OK %c", playerLetter);
            message_send(from, okMessage);
            mem_free(okMessage);

            // Send GRID message
            int nrows = grid_numrows(game_masterGrid(game));
            int ncols = grid_numcols(game_masterGrid(game));
            int rowsDigitLength = snprintf(NULL, 0, "%d", nrows);
            int colsDigitLength = snprintf(NULL, 0, "%d", ncols);
            char* gridMessage = mem_malloc((sizeof(char) * strlen("GRID 1 1")) + rowsDigitLength + colsDigitLength + 1);
            sprintf(gridMessage, "GRID %d %d", nrows, ncols);
            message_send(from, gridMessage);
            mem_free(gridMessage);

            // Send GOLD message
            int r = game_getGold(game);
            int digitLength = snprintf(NULL, 0, "%d", r);
            char* goldMessage = mem_malloc((sizeof(char) * strlen("GOLD 1 1 1")) + digitLength + 1);
            sprintf(goldMessage, "GOLD 0 0 %d", r);
            message_send(from, goldMessage);
            mem_free(goldMessage);

            // Send DISPLAY message
            player_updateVisibleGrid(player, game_masterGrid(game));
            char* string = grid_getDisplay(player_getVisibleGrid(player));
            char* displayMessage = mem_malloc((sizeof(char) * (strlen("DISPLAY\n") + strlen(string))) + 1);
            sprintf(displayMessage, "DISPLAY\n%s", string);
            message_send(from, displayMessage);
            mem_free(displayMessage);
            mem_free(string);

            // Also send DISPLAY message to SPECTATOR if SPECTATOR exists
            spectator_t* spectator;
            if ((spectator = game_getSpectator(game)) != NULL) {
                addr_t to = spectator_getAddress(spectator);
                char* spectatorString = grid_getDisplay(game_masterGrid(game));
                char* spectatorDisplayMessage = mem_malloc((sizeof(char) * (strlen("DISPLAY\n") + strlen(spectatorString))) + 1);
                sprintf(spectatorDisplayMessage, "DISPLAY\n%s", spectatorString);
                message_send(to, spectatorDisplayMessage);
                mem_free(spectatorDisplayMessage);
                mem_free(spectatorString);
            }

        } else {
            message_send(from, "QUIT Game is full: no more players can join.");
        }
    } else {
        message_send(from, "QUIT Sorry - you must provide player's name.");
    }

}

/**************** handleKey() ****************/
/* Takes an optional pointer, address of the player, and string. 
 * Called in the handleMessage() function.
 * Handles KEY message.
 * Returns true/false based on if the game is over.
 */
static bool handleKey(void* arg, const addr_t from, const char* content) {

  bool gameOver = false;

    // Go through each key case if a key was given
    if(content != NULL) {
        char letter = content[0];
        switch (letter) {
            case 'Q': keyQ(from); break; // quit
            case 'q': keyQ(from); break; // quit
            case 'h': gameOver = game_move(game, from, -1, 0); break; // left
            case 'l': gameOver = game_move(game, from, 1, 0); break; // right
            case 'j': gameOver = game_move(game, from, 0, 1); break; // down
            case 'k': gameOver = game_move(game, from, 0, -1); break; // up
            case 'y': gameOver = game_move(game, from, -1, -1); break; // up and left
            case 'u': gameOver = game_move(game, from, 1, -1); break; // up and right
            case 'b': gameOver = game_move(game, from, -1, 1); break; // down and left
            case 'n': gameOver = game_move(game, from, 1, 1); break; // down and right
            case 'H': gameOver = game_longMove(game, from, -1, 0); break; // LONG left
            case 'L': gameOver = game_longMove(game, from, 1, 0); break; // LONG right
            case 'J': gameOver = game_longMove(game, from, 0, 1); break; // LONG down
            case 'K': gameOver = game_longMove(game, from, 0, -1); break; // LONG up
            case 'Y': gameOver = game_longMove(game, from, -1, -1); break; // LONG up and left
            case 'U': gameOver = game_longMove(game, from, 1, -1); break; // LONG up and right
            case 'B': gameOver = game_longMove(game, from, -1, 1); break; // LONG down and left
            case 'N': gameOver = game_longMove(game, from, 1, 1); break; // LONG down and right
            default: errorMessage(from, content); // key not recognized
        }
    }
    return gameOver;
}

/**************** handleSpectate() ****************/
/* Takes an optional pointer, address of the player, and string. 
 * Called in the handleMessage() function.
 * Handles SPECTATE message from client.
 */
static void handleSpectate(void* arg, const addr_t from, const char* content) {

    // Add spectator to game
    game_addSpectator(game, from);

    // Send GRID message
    int nrows = grid_numrows(game_masterGrid(game));
    int ncols = grid_numcols(game_masterGrid(game));
    int rowsDigitLength = snprintf(NULL, 0, "%d", nrows);
    int colsDigitLength = snprintf(NULL, 0, "%d", ncols);
    char* gridMessage = mem_malloc((sizeof(char) * strlen("GRID 1 1")) + rowsDigitLength + colsDigitLength + 1);
    sprintf(gridMessage, "GRID %d %d", nrows, ncols);
    message_send(from, gridMessage);
    mem_free(gridMessage);

    // Send GOLD message
    int r = game_getGold(game);
    int digitLength = snprintf(NULL, 0, "%d", r);
    char* goldMessage = mem_malloc((sizeof(char) * strlen("GOLD 1 1 1")) + digitLength + 1);
    sprintf(goldMessage, "GOLD 0 0 %d", r);
    message_send(from, goldMessage);
    mem_free(goldMessage);

    // Send DISPLAY message
    char* string = grid_getDisplay(game_masterGrid(game));
    char* displayMessage = mem_malloc((sizeof(char) * (strlen("DISPLAY\n") + strlen(string))) + 1);
    sprintf(displayMessage, "DISPLAY\n%s", string);
    message_send(from, displayMessage);
    mem_free(displayMessage);
    mem_free(string);

}

/**************** keyQ() ****************/
/* Takes address of the player.
 * Called in the handleKey() function.
 * Handles scenario of KEY q and KEY Q
 */
static void keyQ(const addr_t from) {

    player_t* player = game_findPlayer(game, from);

     // If the player doesn't exist, then that should mean a spectator pressed Q instead.
    if (player == NULL) {
        game_removeSpectator(game, from); // handles QUIT messaging
    // If the player does exist, remove them
    } else {
        game_removePlayer(game, player); // handles QUIT messaging
    }

}

/**************** errorMessage() ****************/
/* Takes address of the player and string.
 * Called in the handleKey() function.
 * Handles scenario of invalid KEY.
 */
static void errorMessage(const addr_t from, const char* content) {

    // Create error message for when key isn't recognized
    char* errorMsg = malloc((sizeof(char) * (strlen("ERROR - key ' ' not recognized")) + strlen(content)) + 1);
    sprintf(errorMsg, "ERROR - key '%s' not recognized", content);
    message_send(from, errorMsg);
    free(errorMsg);

}

/**************** checkWhitespace() ****************/
/* Takes a string.
 * Returns true if the string is just whitespace, false if not.
 */
static bool checkWhitespace(const char* name){

    for (int i = 0; name[i] != '\0'; i++) {
        if (isalpha(name[i])) {
            // Return false if the string contains letters
            return false;
        }
    }
    // Return true if the string is just whitespace
    return true;

}

/**************** fixName() ****************/
/* Takes a string.
 * Truncates string based on max name length and replaces
 * with underscores 
 * User must free the produced string!
 */
static char* fixName(const char* entry) {

    char* name = mem_malloc(MAXNAMELENGTH+1);
    // If the entry is less than the max name length
    if (strlen(entry) < MAXNAMELENGTH) {
        for (int i = 0; entry[i] != '\0'; i++) {
            // Replace with underscore
            if (!isgraph(entry[i]) && !isblank(entry[i])) {
                name[i] = '_';
            } else {
                name[i] = entry[i];
            }
        }
        name[strlen(entry)] = '\0';
    // If the entry exceeds or matches the max name length
    } else {
        for (int i = 0; i < (MAXNAMELENGTH); i++) {
            // Replace with underscore
            if (!isgraph(entry[i]) && !isblank(entry[i])) {
                name[i] = '_';
            } else {
                name[i] = entry[i];
            }
        }
        name[MAXNAMELENGTH] = '\0';
    }
    return name;
}

