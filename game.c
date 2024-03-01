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
#include "mapchars.h"

// Global Variables
static const MaxNameLength = 50;        // max number of chars in playerName
static const  MaxPlayers = 26;          // maximum number of players
static int GoldTotal = 250;                 // amount of gold in the game and it is not static because it changes later
static const GoldMinNumPiles = 10;      // minimum number of gold piles
static const GoldMaxNumPiles = 30;      // maximum number of gold piles

typedef struct game{
    player_t** players;         // array of players
    grid_t* masterGrid;         // the master grid(map) which covers the whole map
    spectator_t* spectator;     // the address of the spectator
    int numPlayer;              // number of players joined the game so far
    int goldRemain;             // the remaining gold in the game
//    int numPiles;               // the random number of piles between max and min number given as global variables.
} game_t;


// this function initializes the whole game by initializing each small other small part of it.
game_t* game_init(FILE* mapfile){

    game_t* game = mem_malloc_assert(sizeof(game_t), "Failed to allocate memory for game.\n");
    
    // initialize grid
    game->masterGrid = grid_fromMap(mapfile);
    
    // which is between max and min number of piles give as global variable.
    game->numPlayer = 0;// set num of player to zer0
    game->goldRemain = GoldTotal;// set the gold remaining in the game to GoldTotal.


    // initialize gold and set it randomly on the map. 
    if (grid_nuggetsPopulate(game->masterGrid, GoldMinNumPiles, GoldMaxNumPiles,game->goldRemain)){
        flog_v(stderr, "Could not initialie the gold in random spots.\n");
    }
    

    // initialize player 
    game->players = mem_calloc_assert(MaxPlayers, sizeof(player_t*), "Failed to allocate memory for Player.\n"); 

    // initialize spectator
    game->spectator = mem_malloc_assert(sizeof(spectator_t), "Failed to allocate memory for Player.\n");


    return game;
}


// to add a player to the game.Check game.h for more information.
void game_addPlayer(game_t* game, player_t* player){
    if (game != NULL && player != NULL){
        if (game->numPlayer < MaxPlayers){
            game->players[game->numPlayer] = player;
            game->numPlayer++;

            char letter = player_getletter(player);

            grid_addPlayer(game->masterGrid, player_getX(player), player_getY(player), letter);
            
            // make and send message
            // player_sendMessage(player, "OK \n");
        }
        else{
            player_sendMessage(player, "QUIT Game is full: no more players can join.\n");
        }
    }
}

// to remove player from the game. Check game.h for more information.
void game_removePlayer(game_t* game, player_t* playerA){
    if (game != NULL && playerA != NULL){
        player_t* playerB = game_findPlayer(game,player_getAddress(playerA)); 
        if(playerB == NULL){
            flogv(stderr, "Cannot remove a player that is not in players array.\n");
            return;
        }
        player_sendMessage(playerA,"QUIT Thanks for playing!\n");
        player_setInactive(playerA);

        grid_removePlayer(game->masterGrid, player_getletter(playerA), player_getX(playerA), player_getY(playerA));
    }
}

// to add a spectator to the game. Check game.h for more information.
void game_addSpectator(game_t* game, addr_t* address){
    if (game != NULL){
        if (game->spectator != NULL){ // it means that there is already a spepctator in the game
            spectator_sendMessage(game->spectator, "QUIT You have been replaced by a new spectator.\n");
            spectator_delete(game->spectator);
        }
        game-> spectator = spectator_new(address);
    }
}

// to remove spectator from the game. Check game.h for more information.
void game_removeSpectator(game_t* game, addr_t* address){
    if (game != NULL && address != NULL){

        // to check if the spectator is already in the game
        spectator_t* spectator = game->spectator;
        if (!(message_eqAddr(*spectator_getAddress(spectator), *address))){
            flogv(stderr,"cannot remove a spectator that is not in the game.\n");
            return;
        }
        spectator_sendMessage(spectator,"QUIT Thanks for watching!\n");
        spectator_delete(spectator);
        game->spectator = NULL;
    }
}


// to get the players array. Check game.h for more information.
player_t** game_getPlayers(game_t* game){
    if (game == NULL){
        flogv(stderr, "Cannot get the players array of null game.\n");
    }
    return game->players;
}

// to get the spectator. Check game.h for more information.
spectator_t* game_getSpectator(game_t* game){
    if (game == NULL){
        flogv(stderr, "Cannot get the spectator of null game.\n");
    }
    return game->spectator;
}


// to find a player by its address. Check game.h for more information.
player_t* game_findPlayer(game_t* game, addr_t* address){
    if (game == NULL){
        flogv(stderr, "Cannot find player in null game");
        return NULL;
    }
    else{
        player_t* player;
        for (int i = 0; i < game->numPlayer; i++){
            player = game->players[i];
            if (message_eqAddr(*player_getAddress(player), *address)){
                return player;
            }
        }
        // after we come out of loop and could not find the player in players array
        flogv(stderr, "There is no pplayers in array with the given address.\n");
        return NULL;
    }
}



// to change the coordinates and visble grid of player once it moves. 
void game_move(game_t* game, addr_t* address, int dx, int dy){
    if (dx > 1 || dx <-1 || dy > 1 || dy <-1){
        flog_v(stderr, "The coordinates to move the player is not with [-1, +1].\n");
        return;
    }
    if (game == NULL || address == NULL){
        flog_v(stderr, "Cannot move player. Either Null player or Null game c.\n");
        return;
    }
    player_t* player = game_findPlayer(game, address);

    if (player == NULL || !player_isActive(player)){
        flog_v(stderr, "Player not in  game\n");
        return;
    }
    
    // to get the current x and y position of the player
    int px = player_getX(player);
    int py = player_getY(player);
    int claimedGold = grid_movePlayer(game->masterGrid, px, py, dx, dy);
    
    // to update the coordinates
    if (claimedGold != -1) {
        player_moveDiagonal(player, dx, dy);
    }

    // to get the updated x and y position of the player 
    px = player_getX(player);
    py = player_getY(player);
    
    // to update the gold claimed by the player to new coordinates if the player steped on a gold pile.
    if (claimedGold > 0){
        player_addGold(player, claimedGold);

        int purse = player_getGold(player); // the amount of gold player has in its purse.
        game->goldRemain -= claimedGold;
        int remain = game->goldRemain;      // the amount of gold remaining in the game.
        game_sendAllGoldMessages(game);
    }

    // update the visible grids for each player
    game_updateAllVisibleGrids(game);
    game_displayAllPlayers(game);

}


// this functin is to move the player a long as it can move only to one direction, either diagonally, vertically or horizontally.
void game_longMove(game_t* game,addr_t* address, int dx ,int dy){
    if (dx > 1 || dx <-1 || dy > 1 || dy <-1){
        flog_v(stderr, "The coordinates to long move the player is not with [-1, +1].\n");
        return;
    }
    if (game == NULL || address == NULL){
        flog_v(stderr, "Cannot long move player. Either Null player or Null game c.\n");
        return;
    }

    player_t* player = game_findPlayer(game, address);

    if (player == NULL || !player_isActive(player)){
        flog_v(stderr, "Player not in  game\n");
        return;
    }

    int returnVal; // return value from move; is -1 if move failed
    int goldCollected = 0;
    while ((returnVal = grid_movePlayer(game->masterGrid, player_getX(player), player_getY(player), dx, dy)) != -1) {
        player_moveDiagonal(player, dx, dy);
        goldCollected += returnVal;
    }

    if (goldCollected > 0){
        player_addGold(player, goldCollected);

        int purse = player_getGold(player); // the amount of gold player has in its purse.
        game->goldRemain -= goldCollected;
        int remain = game->goldRemain;      // the amount of gold remaining in the game.

        game_sendAllGoldMessages(game,player,goldCollected);
//      char message[100];
//      snprintf(message, sizeof(message), "GOLD %d %d %d", goldCollected, purse, remain);
//      player_sendMessage(player, message);
    }



    // update the visible grids for each player
    game_updateAllVisibleGrids(game);
    game_displayAllPlayers(game);

}
// this is to send the amount of gold the player got, the remaing gold in the game, in thier purse
static void game_sendAllGoldMessages(game_t* game, player_t* player, int playerGold){

    for (int i = 0; i < game->numPlayer; ++i){
        if (Player_iActive(game->players[i])){

            int goldCollected = 0;
            int purse = player_getGold(game->players[i]);
            int remain = game->goldRemain;

            if (game_findPlayer(game, player_getAddress(player)) !=NULL){ // to make sure the current players claimed gold is not zero
                int goldCollected = playerGold;
            }
            
            char message[100];
            snprintf(message, sizeof(message), "GOLD %d %d %d", goldCollected, purse, remain);
            player_sendMessage(game->players[i], message);

        }
    }
}

// to update the visible grid of each player 
void static game_updateAllVisibleGrids(game_t* game){
    for (int i = 0; i < game->numPlayer; ++i){
        player_t* curr = game->players[i];
        if (!player_isActive(curr)){
            continue;
        }
        player_setVisibleGrid(curr, grid_generateVisibleGrid(game->masterGrid, player_getVisibleGrid(curr), player_getX(curr), player_getY(curr)));
    }

}

// A helper funciton to for printing the last results.
void static print_result(player_t* player){// how to manage the new line added by each log here?---------------------
    if (player != NULL){
        flog_c(stderr, "%c",player_getletter(player));
        flog_d(stderr, "    %d ", player_getGold(player));
        flog_s(stderr,"%s", player_getName(player));
        char* result = ("%c    %d %s",player_getletter(player), player_getGold(player), player_getName(player));
        flog_v(stderr, result);
    }
}


// to delete everything in the game that was initialized before. Check game.h for more information. 
void game_over(game_t* game){

    /************* delete players ************/
    // to delete each player one by one
    for (int i = 0; i < game->numPlayer; i++){
        print_result(game->players[i]);
        player_delete(game->players[i]);
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

// TODO
// player_isActive ++++++
// game_sendAllGoldMessages +++++++
// game_displayAllPlayers(game); 

