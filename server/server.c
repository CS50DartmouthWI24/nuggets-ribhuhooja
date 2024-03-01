#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "player.h"
// #include "file.h"
#include "message.h"
// #include "grid.h"

static void parseArgs(const int argc, char* argv[],
                      char** map, char** seed);
static bool handleMessage(void* arg, const addr_t from, const char* buf);
static void countMapDimensions(FILE* fp, int* ncolsp, int* nrowsp);
static void handlePlay(void* arg, const addr_t from, const char* content);
static void handleKey(void* arg, const addr_t from, const char* content);
static void handleSpectate(void* arg, const addr_t from, const char* content);
static void keyQ(void* arg, const addr_t from, const char* content);

const int MAXNAMELENGTH = 50;   // max number of chars in playerName
const int MAXPLAYERS = 26;      // maximum number of players
const int GOLDTOTAL = 250;      // amount of gold in the game
const int GoldMinNumPiles = 10; // minimum number of gold piles
const int GoldMaxNumPiles = 30; // maximum number of gold piles
const int TIMEOUT = 15;

const int ncols;
const int nrows;

// game_t game;

/**************** main() ****************/
int main(const int argc, char* argv[]) {

    char* map = NULL;
    char* seed = NULL;

    parseArgs(argc, argv, &map, &seed);
    // game = game_init(map);

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
                      char** map, char** seed) {

    if (argc == 2 || argc == 3){

        *map = argv[1];

        FILE* fp;

        if ((fp = fopen(*map, "r")) == NULL) {
            fprintf(stderr, "ERROR: Couldn't read file at '%s'\n", *map);
            exit(2);
        }


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

    // // Assumes fp is already open.

    // // Set numbers. ncols is 0 for now.
    // *ncolsp = 0;
    // *nrowsp = file_numLines(fp);

    // char* line;

    // // Go through each line
    // while((line = file_readLine(fp)) != NULL) {
    //     // If the number of chars in this line exceeds the current ncols
    //     if(strlen(line) > *ncolsp){
    //         // Make it the new ncols. This way we find the maximum ncols.
    //         *ncolsp = strlen(line);
    //     }
    // }
}

static bool handleMessage(void* arg, const addr_t from, const char* message) {

    // char* message = buf;

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

    return false;

}

static void handlePlay(void* arg, const addr_t from, const char* content) {

    // SYNTAX: PLAY real name
    const addr_t* to = &from;
    char* name = content;
    printf("Player name: %s\n", name);
    player_t* player = player_new(to, 0, 0, name, 'A');
    printf("%s\n", player_getName(player));
    char* display = "DISPLAY\n123456789\n123456789\n123456789\n";
    message_send(from, display);
    
    // if(content != NULL) {
    //    // if playerCount != max
    //     int x = randomNumber(0, ncols);
    //     int y = randomNumber(0, nrows);
    //     player_t* player = player_new(x, y, content, from);
    //     char playerLetter = 'A' + game.numPlayer;
    //     player->letter = playerLetter;
    //     game_addPlayer(game, player);
    //     message_send(from, "OK %c", playerLetter);
    //     message_send(from, "GRID %d %d", nrows, ncols);
    //     // else
    //     message_send(from, "QUIT Game is full: no more players can join.");

    // } else {
    //     message_send(from, "QUIT Sorry - you must provide player's name.");
    // }

}

static void handleKey(void* arg, const addr_t from, const char* content) {

    // SYNTAX: KEY k

    if(content != NULL) {
        char letter = content;
        switch (letter) {
            case 'Q': keyQ(); break; // quit
            case 'q': keyQ(); break; // quit
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
            default: message_send(from, "ERROR - key not recognized.")
        }
    }

}

static void handleSpectate(void* arg, const addr_t from, const char* content) {

    // Add spectator to game
    game_addspectator(from, content);

    // Send grid message
    char* gridMessage = malloc((sizeof(char) * strlen("GRID 1 1")) + 1)
    sprintf(gridMessage, "GRID %d %d", nrows, ncols);
    message_send(from, gridMessage);
    // Send gold message
    // need function to find player from address int n = player_getGold()
    char* goldMessage = malloc((sizeof(char) * strlen("GOLD 1 1 1")) + 1)
    sprintf(goldMessage, "GOLD %d %d %d", n, p, r);
    message_send(from, goldMessage);
    // Send full map
    char* displayMessage = malloc((sizeof(char) * (strlen("DISPLAY\n") + strlen(grid_getDisplay(game.grid)))) + 1)
    char* string = grid_getDisplay(game.grid);
    sprintf(displayMessage, "DISPLAY\n%s", string);
    message_send(from, displayMessage);

}

static void keyQ(void* arg, const addr_t from, const char* content) { // QUIT

}
