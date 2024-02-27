#include <stdio.h>
#include <string.h>

#include "file.h"
#include "message.h"

const int MAXNAMELENGTH = 50;   // max number of chars in playerName
const int MAXPLAYERS = 26;      // maximum number of players
const int GOLDTOTAL = 250;      // amount of gold in the game
const int GoldMinNumPiles = 10; // minimum number of gold piles
const int GoldMaxNumPiles = 30; // maximum number of gold piles
const int TIMEOUT = 15;
char letters[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

int ncols;
int nrows;

game_t* game;

/**************** main() ****************/
int main(const int argc, char* argv[]) {

    char* map = NULL;
    char* seed = NULL;
    game_t* game;

    parseArgs(argc, argv, &map, &seed);
    game = game_init(map);

    int port = message_init(stderr);
    if (port == 0) {
        return 2; // failure to initialize message module
    } else {
        printf("serverPort=%d\n", port);
    }
    message_loop(NULL, TIMEOUT, handleTimeout, handleStdin, handleMessage);
    message_done();

}

/* ******************* parseArgs ******************* */
/* Handle the messy business of parsing command-line arguments.
 * We return 'void' because we only return when successful; otherwise,
 * we print error or usage message to stderr and exit non-zero.
 * 
 * Adapted from parseArgs.c on CS50 Github
 */
static void parseArgs(const int argc, char* argv[],
                      char** map, char** seed) {

    // I need to clean this whole thing up

    if (argc == 2 || argc == 3){

        FILE* fp;

        if ((fp = fopen(map, "r")) == NULL) {
            fprintf(stderr, "ERROR: Couldn't read file at '%s'\n", map);
            exit(2);
        }

        *map = argv[1];

        if (argc == 3) {
            // This isn't set up yet
            *seed = argv[2];
        }

        // Find proper ncols, nrows. This might become obsolete once grid is made.
        countMapDimensions(fp, &ncols, &nrows);

        fclose(fp); 

    } else { // Not enough arguments or too many arguments
        fprintf(stderr, "USAGE: server map.txt [seed]\n");
        exit(1);
    }

}

static void countMapDimensions(FILE* fp, int* ncolsp, int* nrowsp) {

    // Assumes fp is already open.

    // Set numbers. ncols is 0 for now.
    *ncolsp = 0;
    *nrowsp = file_numLines(fp);

    char* line;

    // Go through each line
    while((line = file_readLine(fp)) != NULL) {
        // If the number of chars in this line exceeds the current ncols
        if(strlen(line) > *ncolsp){
            // Make it the new ncols. This way we find the maximum ncols.
            *ncolsp = strlen(line);
        }
    }
}

static void handleMessage(void* arg, const addr_t from, const char* buf) {

    char* message = buf;

    if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) { // PLAY
        const char* content = message + strlen("PLAY ");
        handlePlay(arg, from, content);
    } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) { // KEY
        const char* content = message + strlen("PLAY ");
        handleKey(arg, from, content);
    } else if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0) {
        const char* content = message + strlen("SPECTATE");
        handleSpectate(arg, from, content);
    } 

}

static void handlePlay(void* arg, const addr_t from, const char* content) {

    // SYNTAX: PLAY real name

    if(content != NULL) {
        //if playerCount != max
        int x = randomNumber(0, ncols);
        int y = randomNumber(0, nrows);
        player_t* player = player_new(x, y, content, from);
        game_addPlayer(game, player);
        char playerLetter = letters[(game->numPlayer)-1]
        player->letter = playerLetter;
        message_send(from, "OK %c", playerLetter);
        message_send(from, "GRID %d %d", nrows, ncols);
        // else
        message_send(from, "QUIT Game is full: no more players can join.");

    } else {
        message_send(from, "QUIT Sorry - you must provide player's name.");
    }

}

static void handleKey(void* arg, const addr_t from, const char* content) {

    // SYNTAX: KEY k

    if(content != NULL) {
        //
    }

}

static void handleSpectate(void* arg, const addr_t from, const char* content) {

    // Add spectator to game
    game_addspectator(from, content);

    // Send grid message
    message_send(from, "GRID %d %d", nrows, ncols);

    // Send gold message
    // need function to find player from address int n = player_getGold()
    message_send(from, "GOLD %d %d %d", n, p, r);
    // Send full map
    char* string = grid_getDisplay(game->grid);
    message_send(from, "DISPLAY\n%s", string);

}