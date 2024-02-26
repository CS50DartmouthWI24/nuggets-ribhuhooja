/* This file is game.c and and it will contain the main function of the game. 
 * Team torpedos Winter, 2024
 * 
 * Author:Tayeb Mohammadi
 * 
*/


#include <stdio.h>
#include <stdlib.h>
#include "spectator.h"
#include "player.h"
#include "grid.h" 
#include "log.h"
#include "mem.h"
#include "game.h"

// Global Variables
const MaxNameLength = 50;   // max number of chars in playerName
const  MaxPlayers = 26;      // maximum number of players
GoldTotal = 250;      // amount of gold in the game and it is not static because it changes later
const GoldMinNumPiles = 10; // minimum number of gold piles
const GoldMaxNumPiles = 30; // maximum number of gold piles

typedef struct game{
    player_t** players; // array of players
    grid_t* masterGrid; // the master grid(map) which covers the whole map
    spectator_t* spectator; // the address of the spectator
    int numPlayer; // number of players joined the game so far
    int goldRemain; // the remaining gold in the game
    int numPiles; // the random number of piles between max and min number given as global variables.
} game_t;


// to get a random number between max and min numbers
static int randomNumber(int min, int max){
    if (max <= min){
        flogv(stderr,"Failed to generate a random variable for invalid max and min.\n");
        return -1;
    }
    int num = rand();
    num = min + num % (max - min + 1);
    return min;

}

// to get a random number between max and min numbers for gold. 
static int random_goldNumber(grid_t* grid, int numPiles, int count, int remaining){
    if (grid == NULL || numPiles <=0){
        flogv(stderr,"Failed to generate a random gold pile.\n");
        return -1;
    }
    if ((numPiles - count) == 1){ // to allocate all the remaing gold to the last pile on map
        return remaining;
    }
    // if not the last pile of gold, devide the remaining gold by numPiles - count
    int gold = remaining / (numPiles - count); 
    // and then return a random value between 1 and that number
    return randomNumber(1, gold);

}
// this function find a random free spot in grid and set its value in Gridcounter (Nuggests) and put a '*' instead of '.'
void gold_init(grid_t* grid, game_t* game){
    int numPiles = game->numPiles;
    char* string = grid->string;
    int gold;

    int count = 0;
    while (count < numPiles){
        // minimum is 0 and maximum is the length of the string 
        int min = 0;
        int max = strlen(string);
        int random = randomNumber(min , max);
        // to check it the spot is free.
        if (string[count] = '.'){
            string[count] = "*";
            gold = random_goldNumber(grid, numPiles,count, game->goldRemain);
            game->goldRemain -= gold;
            counters_set(grid->nuggets, count, gold);
            count++;
        }
    }
}

// this function initializes the whole game by initializing each small other small part of it.
game_t* game_init(FILE* mapfile){

    game_t* game = mem_malloc_assert(sizeof(game_t), "Failed to allocate memory for game.\n");
    
    // initialize grid
    game->masterGrid = grid_fromMap(mapfile);
    
    game->numPiles = randomNumber(GoldMinNumPiles,GoldMaxNumPiles);// to find a random number for piles of gold
    // which is between max and min number of piles give as global variable.
    game->numPlayer = 0;// set num of player to zer0
    game->goldRemain = GoldTotal;// set the gold remaining in the game to GoldTotal.


    // initialize gold and set it randomly on the map
    gold_init(game->masterGrid, game);

    // initialize player 
    game->players = mem_calloc_assert(26, sizeof(player_t*), "Failed to allocate memory for Player.\n"); // is it a pointer to player or the player itself?------------------

    // initialize spectator
    game->spectator = mem_malloc_assert(sizeof(spectator_t), "Failed to allocate memory for Player.\n");


    return game;
}


// to add a player to the game
void game_addPlayer(game_t* game, player_t* player){// how to check if the player was already in array
    if (game != NULL || player != NULL){
        if (game->numPlayer < 26){
            game->players[game->numPlayer] = player;
            game->numPlayer++;
        }
        else{
            player_sendMessage("QUIT Game is full: no more players can join.\n");
        }
    }
}


// to add a spectator to the game 
void game_addspectator(game_t* game, addr_t* address){
    if (game != NULL){
        if (game->spectator != NULL){ // it means that there is already a spepctator in the game
            spectator_sendMessage(game->spectator, "QUIT You have been replaced by a new spectator.\n");
            spectator_delete(game->spectator);
        }
        spectator_new(address);
    }
}

// to get the players array 
player_t** game_getPlayers(game_t* game){
    if (game == NULL){
        flogv(stderr, "Cannot get the players array of null game.\n");
    }
    return game->players;
}

// to find a player by its address
player_t* game_findPlayer(game_t* game, addr_t address){
    if (game = NULL){
        flogv(stderr, "Cannot find player in null game");
        return NULL;
    }
    else{
        player_t* player;
        for (int i = 0; i < game->numPlayer; i++){
            player = game->players[i];
            if (message_eqAddr(player->address, address)){
                return player;
            }
        }
        // after we come out of loop and could not find the player in players array
        flogv(stderr, "There is no pplayers in array with the given address.\n");
        return NULL;
    }
}

// to delete everything in the game that was initialized before
void game_over(game_t* game){

    /************* delete players ************/
    // to delete each player one by one
    for (int i = 0; i < game->numPlayer; i++){
        player_delete(game->players[game->numPlayer]);
    }
    // and then free the array of players too
    mem_free(game->players);

    // delete spectator
    spectator_delete(game->spectator);
    // delete grid
    grid_delete(game->masterGrid);
    // free game structure
    mem_free(game);

}






