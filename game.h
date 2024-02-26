/* This file is game.c and and it will contain the main function of the game. 
 * Team torpedos Winter, 2024
 * 
 * Author:Tayeb Mohammadi
 * 
*/

// 1. should I have another array for inactive players
// 2. when returning the array of players, what to do with inactive players.


#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "player.h"
#include "spectator.h"





typedef struct game{
    player_t** players; // array of players
    grid_t* masterGrid; // the master grid(map) which covers the whole map
    spectator_t* spectator; // the address of the spectator
    int numPlayer; // number of players joined the game so far
    int goldRemain; // the remaining gold in the game
    int numPiles; // the random number of piles between max and min number given as global variables.
} game_t;



/************* game_init *************/
/**
 * Caller provides: 
 *  @param mapfile as a FILE pointer 
 * 
 * We do: 
 *  Initialize the game by allocating memory for players, spectators.
 *  Initialize master grid and set numPiles.
 *  
 * We return:
 *  The initialized game_t, NULL if any failure.
 * 
 * Notes:
*/
game_t* game_init(FILE* mapfile);


/************* game_addPlayer *************/
/** Add a new player to the game

 * Caller provides: 
 *  @param game structure pointer, 
 *  @param player structure pointer
 * 
 * We do:
 *  if there are less than 26 player in the game and the player has not joined before,
 *  add it to player array. Otherwise, send an error messsage and the request to
 *  join the game should be denied. 
 *  
 * We return:
 *  Null
 * 
 * Notes:
*/
void game_addPlayer(game_t* game, player_t* player);


/************* game_addSpectator *************/
/** Add a new spectator to the game

 * Caller provides: 
 *  @param game structure pointer, 
 *  @param address of spectator
 * 
 * We do:
 *  if there is already a spectator in the game, replace the new spectator 
 *  by the current one and send a quit message to current one and initialize the new spectator. 
 *  Otherwise, just accept the request and initialize the spectator.
 * 
 * We return:
 *  Null
 * 
 * Notes:
*/
void game_addspectator(game_t* game, addr_t address);



/************* game_getPlayers *************/
/** Return the array of players

 * Caller provides: 
 *  @param game structure pointer, 
 * 
 * We do:
 *  Get the array of the players pointers. It is possible for the array to be NULL.
 *  The array is not always full, which means that there are some null spots for the 
 *  empty space on the arrray
 * 
 * We return:
 *  A pointer to the array of players (player_t*)
 * 
 * Notes:
 *  The array can be null with zero player. The caller should take care of that. 
*/
player_t** game_getPlayers(game_t* game);


/************* game_findPlayer *************/
/** Find a player by its address

 * Caller provides: 
 *  @param game structure pointer, 
 *  @param address of the player
 * 
 * We do:
 *  look for the player in the players arrays and get the address of the player
 * 
 * We return:
 *  A pointer to the players (player_t*) if it exist in the array.
 *  Otherwise, null.
 * 
 * Notes:
 *  The array can be null with zero player. The caller should take care of that. 
*/
player_t* game_findPlayer(game_t* game, addr_t address);


/************* game_over *************/
/** Ends the game

 * Caller provides: 
 *  @param game structure pointer 
 * 
 * We do: 
 *  send a quit message to all players and the spectator,
 *  print the total gold claimed by each player, and free all alocated memory.
 *  
 * We return:
 *  Null
 * 
 * Notes:
*/
void game_over(game_t* game);
