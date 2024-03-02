#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "game.h"
#include "player.h"
#include "message.h"
#include "set.h"
#include "grid.h"
#include "mem.h"

static void parseArgs(const int argc, char* argv[],
                      FILE** map, int* seed);
static bool handleMessage(void* arg, const addr_t from, const char* buf);
static void handlePlay(void* arg, const addr_t from, const char* content);
static void handleKey(void* arg, const addr_t from, const char* content);
static void handleSpectate(void* arg, const addr_t from, const char* content);
static void keyQ(const addr_t from);
static void errorMessage(const addr_t from, const char* content);

const int MAXNAMELENGTH = 50;   // max number of chars in playerName
const int MAXPLAYERS = 26;      // maximum number of players
const int GOLDTOTAL = 250;      // amount of gold in the game
const int GoldMinNumPiles = 10; // minimum number of gold piles
const int GoldMaxNumPiles = 30; // maximum number of gold piles
const int TIMEOUT = 15;

game_t* game;

/**************** main() ****************/
int main(const int argc, char* argv[]) {

    FILE* map = NULL;
    int seed;

    parseArgs(argc, argv, &map, &seed);
    game = game_init(map);

    // const int ncols = grid_numcols(game_masterGrid(game));
    // const int nrows = grid_numrows(game_masterGrid(game));

    int port = message_init(stderr);
    if (port == 0) {
        return 2; // failure to initialize message module
    } else {
        printf("serverPort=%d\n", port);
    }

    bool ok = message_loop(NULL, 0, NULL, NULL, handleMessage);

    message_done();

    return ok? 0 : 1;

}

/* ******************* parseArgs ******************* */
/* Handle the messy business of parsing command-line arguments.
 * We return 'void' because we only return when successful; otherwise,
 * we print error or usage message to stderr and exit non-zero.
 * 
 * Adapted from parseArgs.c on CS50 Github
 */
static void parseArgs(const int argc, char* argv[],
                      FILE** map, int* seed) {

    if (argc == 2 || argc == 3){

        char* mapString = argv[1];

        if ((*map = fopen(mapString, "r")) == NULL) {
            fprintf(stderr, "ERROR: Couldn't read file at '%s'\n", mapString);
            exit(2);
        }

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
            }
        } else {
            srand(getpid());
        }

        // fclose(fp); 

    } else { // Not enough arguments or too many arguments
        fprintf(stderr, "USAGE: server map.txt [seed]\n");
        exit(1);
    }

}

static bool handleMessage(void* arg, const addr_t from, const char* message) {

    // char* message = buf;

    if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) { // PLAY
        const char* content = message + strlen("PLAY ");
        handlePlay(arg, from, content);
    } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) { // KEY
        const char* content = message + strlen("KEY ");
        handleKey(arg, from, content);
    } else if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0) {
        const char* content = message + strlen("SPECTATE");
        handleSpectate(arg, from, content);
    } 

    return false;

}

static void handlePlay(void* arg, const addr_t from, const char* content) {

    // SYNTAX: PLAY real name
    
    if(content != NULL) {
        if (game_numPlayers(game) != MAXPLAYERS) {

            // Intialize and add player
            int x;
            int y;
            grid_findRandomSpawnPosition(game_masterGrid(game), &x, &y);
            char playerLetter = 'A' + game_numPlayers(game);
            const char* name = content;
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
            char* string = grid_getDisplay(game_masterGrid(game));
            char* displayMessage = mem_malloc((sizeof(char) * (strlen("DISPLAY\n") + strlen(string))) + 1);
            sprintf(displayMessage, "DISPLAY\n%s", string);
            message_send(from, displayMessage);
            mem_free(displayMessage);
            mem_free(string);

        } else {
            message_send(from, "QUIT Game is full: no more players can join.");
        }
    } else {
        message_send(from, "QUIT Sorry - you must provide player's name.");
    }

}

static void handleKey(void* arg, const addr_t from, const char* content) {

    // SYNTAX: KEY k

    if(content != NULL) {
        char letter = content[0];
        fprintf(stderr, "letter: %c\n", letter);
        switch (letter) {
            case 'Q': keyQ(from); break; // quit
            case 'q': keyQ(from); break; // quit
            case 'h': game_move(game, from, -1, 0); break; // left
            case 'l': game_move(game, from, 1, 0); break; // right
            case 'j': game_move(game, from, 0, 1); break; // down
            case 'k': game_move(game, from, 0, -1); break; // up
            case 'y': game_move(game, from, -1, -1); break; // up and left
            case 'u': game_move(game, from, 1, -1); break; // up and right
            case 'b': game_move(game, from, -1, 1); break; // down and left
            case 'n': game_move(game, from, 1, 1); break; // down and right
            case 'H': game_longMove(game, from, -1, 0); break; // LONG left
            case 'L': game_longMove(game, from, 1, 0); break; // LONG right
            case 'J': game_longMove(game, from, 0, 1); break; // LONG down
            case 'K': game_longMove(game, from, 0, -1); break; // LONG up
            case 'Y': game_longMove(game, from, -1, -1); break; // LONG up and left
            case 'U': game_longMove(game, from, 1, -1); break; // LONG up and right
            case 'B': game_longMove(game, from, -1, 1); break; // LONG down and left
            case 'N': game_longMove(game, from, 1, 1); break; // LONG down and right
            default: errorMessage(from, content);
        }

    }

}

static void handleSpectate(void* arg, const addr_t from, const char* content) {

    // Add spectator to game
    game_addSpectator(game, from);

    // Send grid message
    int nrows = grid_numrows(game_masterGrid(game));
    int ncols = grid_numcols(game_masterGrid(game));
    int rowsDigitLength = snprintf(NULL, 0, "%d", nrows);
    int colsDigitLength = snprintf(NULL, 0, "%d", ncols);
    char* gridMessage = mem_malloc((sizeof(char) * strlen("GRID 1 1")) + rowsDigitLength + colsDigitLength + 1);
    sprintf(gridMessage, "GRID %d %d", nrows, ncols);
    message_send(from, gridMessage);
    mem_free(gridMessage);

    // Send gold message
    int r = game_getGold(game);
    int digitLength = snprintf(NULL, 0, "%d", r);
    char* goldMessage = mem_malloc((sizeof(char) * strlen("GOLD 1 1 1")) + digitLength + 1);
    sprintf(goldMessage, "GOLD 0 0 %d", r);
    message_send(from, goldMessage);
    mem_free(goldMessage);

    // Send full map
    char* string = grid_getDisplay(game_masterGrid(game));
    char* displayMessage = mem_malloc((sizeof(char) * (strlen("DISPLAY\n") + strlen(string))) + 1);
    sprintf(displayMessage, "DISPLAY\n%s", string);
    message_send(from, displayMessage);
    mem_free(displayMessage);
    mem_free(string);

}

static void keyQ(const addr_t from) { // QUIT

    player_t* player = game_findPlayer(game, from);

    if (player == NULL) { // If the player doesn't exist, then that should mean a spectator pressed Q instead.
        game_removeSpectator(game, from); // handles QUIT messaging
    } else { // If the player does exist, remove them
        game_removePlayer(game, player); // handles QUIT messaging
    }

}

static void errorMessage(const addr_t from, const char* content) {

    char* errorMsg = malloc((sizeof(char) * (strlen("ERROR - key ' ' not recognized")) + strlen(content)) + 1);
    sprintf(errorMsg, "ERROR - key '%s' not recognized", content);
    message_send(from, errorMsg);

}
