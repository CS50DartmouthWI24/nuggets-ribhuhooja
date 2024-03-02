/* This file is game.c and and it will contain the main function of the game. 
 * Team torpedos Winter, 2024
 * 
 * Author:Tayeb Mohammadi
 *
 * Modified: Ribhu Hooja, March 2024
 * 
*/


#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "mem.h"
#include "spectator.h"
#include "player.h"
#include "grid.h" 
#include "game.h"
#include "mapchars.h"

// Global Constants
static const int MaxNameLength = 50;        // max number of chars in playerName
static const int MaxPlayers = 26;           // maximum number of players
static const int GoldTotal = 250;           // amount of gold in the game
static const int GoldMinNumPiles = 10;      // minimum number of gold piles
static const int GoldMaxNumPiles = 30;      // maximum number of gold piles

/****************** the game type ************************/
typedef struct game{
    player_t** players;         // array of players
    grid_t* masterGrid;         // the master grid(map) which covers the whole map
    spectator_t* spectator;     // the address of the spectator
    int numPlayer;              // number of players joined the game so far
    int goldRemain;             // the remaining gold in the game
} game_t;

/****************** local functions **********************/
static void sendGoldMessage(game_t* game, player_t* player, const int goldCollected, const int purse, const int goldRemaining);
static void sendAllGoldMessages(game_t* game, player_t* goldJustCollectedPlayer, int goldJustCollected);
static void displayAllPlayers(game_t* game);
static void updateAllVisibleGrids(game_t* game);
static char* get_result(game_t* game);
static player_t* findPlayerByCoords(game_t* game, const int x, const int y);



// this function initializes the whole game by initializing 
// each small part of it.
game_t* game_init(FILE* mapfile){

    game_t* game = mem_malloc_assert(sizeof(game_t), "Failed to allocate memory for game.\n");
    
    // initialize grid
    game->masterGrid = grid_fromMap(mapfile);
    
    // which is between max and min number of piles give as global variable.
    game->numPlayer = 0;// set num of player to zero
    game->goldRemain = GoldTotal;// set the gold remaining in the game to GoldTotal.


    // initialize gold and set it randomly on the map. 
    if (!grid_nuggetsPopulate(game->masterGrid, GoldMinNumPiles, GoldMaxNumPiles,game->goldRemain)){
        flog_v(stderr, "Could not initialie the gold in random spots.\n");
    }
    

    // initialize player 
    game->players = mem_calloc_assert(MaxPlayers, sizeof(player_t*), "Failed to allocate memory for Player.\n"); 

    // initialize spectator
    game->spectator = NULL;
    return game;
}


// to add a player to the game.
// Check game.h for more information.
void game_addPlayer(game_t* game, player_t* player){
    if (game != NULL && player != NULL){
        if (game->numPlayer < MaxPlayers){
            game->players[game->numPlayer] = player;
            game->numPlayer++;

            char letter = player_getLetter(player);

            grid_addPlayer(game->masterGrid, player_getX(player), player_getY(player), letter);
            
            // make and send message
            // player_sendMessage(player, "OK \n");
        }
        else{
            player_sendMessage(player, "QUIT Game is full: no more players can join.\n");
        }
    }
}

// to remove player from the game.
// Check game.h for more information.
void game_removePlayer(game_t* game, player_t* playerA){
    if (game != NULL && playerA != NULL){
        player_t* playerB = game_findPlayer(game,player_getAddress(playerA)); 
        if(playerB == NULL){
            flog_v(stderr, "Cannot remove a player that is not in players array.\n");
            return;
        }
        player_sendMessage(playerA,"QUIT Thanks for playing!\n");
        player_setInactive(playerA);

        grid_removePlayer(game->masterGrid, player_getLetter(playerA), player_getX(playerA), player_getY(playerA));
    }
}

// to add a spectator to the game.
// Check game.h for more information.
void game_addSpectator(game_t* game, addr_t address){
    if (game != NULL){
        if (game->spectator != NULL){ 
            spectator_sendMessage(game->spectator, "QUIT You have been replaced by a new spectator.\n");
            spectator_delete(game->spectator);
        }
        game-> spectator = spectator_new(address);
    }
}

// to remove spectator from the game. Check game.h for more information.
void game_removeSpectator(game_t* game, addr_t address){
    if (game != NULL){

        // to check if the spectator is already in the game
        spectator_t* spectator = game->spectator;
        if (!(message_eqAddr(spectator_getAddress(spectator), address))){
            flog_v(stderr,"cannot remove a spectator that is not in the game.\n");
            return;
        }
        spectator_sendMessage(spectator,"QUIT Thanks for watching!\n");
        spectator_delete(spectator);
        game->spectator = NULL;
    }
}

/****************** game_masterGrid ***********************
 * 
 * see game.h for description and usage
 *
 */
grid_t*
game_masterGrid(game_t* game)
{
  if (game == NULL){
    return NULL;
  }
  
  return game->masterGrid;
}

/****************** game_numPlayers ***********************
 *
 * see game.h for description and usage
 *
 */
int
game_numPlayers(game_t* game)
{
  if (game == NULL){
    return 0;
  }

  return game->numPlayer;
}

// to get the players array. Check game.h for more information.
player_t** game_getPlayers(game_t* game){
    if (game == NULL){
        flog_v(stderr, "Cannot get the players array of null game.\n");
    }
    return game->players;
}

// to get the players array. Check game.h for more information.
int game_getGold(game_t* game){
    if (game == NULL){
        flog_v(stderr, "Cannot get the amount of gold of null game.\n");
        return -1;
    }
    return game->goldRemain;
}


// to get the spectator. Check game.h for more information.
spectator_t* game_getSpectator(game_t* game){
    if (game == NULL){
        flog_v(stderr, "Cannot get the spectator of null game.\n");
    }
    return game->spectator;
}


// to find a player by its address. Check game.h for more information.
player_t* game_findPlayer(game_t* game, addr_t address){
    if (game == NULL){
        flog_v(stderr, "Cannot find player in null game");
        return NULL;
    }
    else{
        player_t* player;
        for (int i = 0; i < game->numPlayer; i++){
            player = game->players[i];
            if (message_eqAddr(player_getAddress(player), address)){
                return player;
            }
        }
        // after we come out of loop and could not find the player in players array
        flog_v(stderr, "There is no pplayers in array with the given address.\n");
        return NULL;
    }
}



// to change the coordinates and visble grid of player once it moves. 
bool game_move(game_t* game, addr_t address, int dx, int dy){
    if (dx > 1 || dx <-1 || dy > 1 || dy <-1){
        flog_v(stderr, "The coordinates to move the player is not with [-1, +1].\n");
        return false;
    }

    if (game == NULL){
        flog_v(stderr, "Cannot move player. Either Null player or Null game c.\n");
        return false;
    }
    player_t* player = game_findPlayer(game, address);

    if (player == NULL || !player_isActive(player)){
        flog_v(stderr, "Player not in  game\n");
        return false;
    }
    
    // to get the current x and y position of the player
    int px = player_getX(player);
    int py = player_getY(player);
    int returnVal = grid_movePlayer(game->masterGrid, px, py, dx, dy);

    
    if (returnVal == -1){   // no move
      return false;
    } else if (returnVal == -2) {   // player there; swap!
      grid_swapPlayers(game->masterGrid, px, py, px + dx, py + dy);
      player_moveDiagonal(player, dx, dy);
      player_t* other = findPlayerByCoords(game, px + dx, py + dy);
      if (other != NULL){
        player_setX(other, px);
        player_setY(other, py);
      }

      return false;
    } else {
      player_moveDiagonal(player, dx, dy);
    }

    // to get the updated x and y position of the player 
    px = player_getX(player);
    py = player_getY(player);
    
    // to update the gold claimed by the player to new coordinates if the player steped on a gold pile.
    if (returnVal > 0){
        int claimedGold = returnVal;
        player_addGold(player, claimedGold);

        //int purse = player_getGold(player); // the amount of gold player has in its purse.
        game->goldRemain -= claimedGold;
        // int remain = game->goldRemain;      // the amount of gold remaining in the game.
        sendAllGoldMessages(game, player, claimedGold);
    }

    // update the visible grids for each player

    updateAllVisibleGrids(game);
    displayAllPlayers(game);
    
    if (game->goldRemain == 0){
       game_over(game);
       return true;
    }

    return false;
}


// this functin is to move the player a long as it can move only to one direction, either diagonally, vertically or horizontally.
bool game_longMove(game_t* game,addr_t address, int dx ,int dy){
    if (dx > 1 || dx <-1 || dy > 1 || dy <-1){
        flog_v(stderr, "The coordinates to long move the player is not with [-1, +1].\n");
        return false;
    }
    if (game == NULL){
        flog_v(stderr, "Cannot long move player. Either Null player or Null game c.\n");
        return false;
    }

    player_t* player = game_findPlayer(game, address);

    if (player == NULL || !player_isActive(player)){
        flog_v(stderr, "Player not in  game\n");
        return false;
    }

    int px = player_getX(player);
    int py = player_getY(player);

    int returnVal; // return value from move; is -1 if move failed
    int goldCollected = 0;
    while ((returnVal = grid_movePlayer(game->masterGrid, player_getX(player), player_getY(player), dx, dy)) != -1) {
        if (returnVal == -2){
            grid_swapPlayers(game->masterGrid, px, py, px + dx, py + dy);
            player_t* other = findPlayerByCoords(game, px + dx, py + dy);
            if (other != NULL){
                player_setX(other, px);
                player_setY(other, py);
            }
        }

        player_moveDiagonal(player, dx, dy);
        goldCollected += returnVal;

        player_updateVisibleGrid(player, game->masterGrid);
    }

    if (goldCollected > 0){
        player_addGold(player, goldCollected);
        game->goldRemain -= goldCollected;
        sendAllGoldMessages(game, player, goldCollected);
    }



    // update the visible grids for each player
    updateAllVisibleGrids(game);
    displayAllPlayers(game);

    if (game->goldRemain == 0){
       game_over(game);
       return true;
    }

    return false;
}

// this is to send the amount of gold the player got, the remaing gold in the game, in thier purse
static void sendAllGoldMessages(game_t* game, player_t* goldJustCollectedPlayer, int goldJustCollected){

    if (game == NULL || goldJustCollectedPlayer == NULL){
        return;
    }

    int remain = game->goldRemain;
    addr_t address = player_getAddress(goldJustCollectedPlayer);
    for (int i = 0; i < game->numPlayer; ++i){
        player_t* player = game->players[i];
        if (player_isActive(player)){

            int goldCollected = 0;
            int purse = player_getGold(player);

            if (message_eqAddr(address, player_getAddress(player))){
              goldCollected = goldJustCollected;

            }

            sendGoldMessage(game, player, goldCollected, purse,
                                                         remain);
        }
    }
}

/****************** sendGoldMessage ***********************
 *
 * sends a "GOLD n p r" message to the player
 *
 */
static void sendGoldMessage(game_t* game, player_t* player, const int goldCollected, 
                                                            const int purse,
                                                            const int remaining)
{
  char message[100];
  snprintf(message, sizeof(message), "GOLD %d %d %d", goldCollected, purse, remaining);
  player_sendMessage(player, message);
}



// to update the visible grid of each player 
static void updateAllVisibleGrids(game_t* game){
  if (game == NULL){
    return;
  }

  for (int i = 0; i < game->numPlayer; ++i){
      player_t* curr = game->players[i];
      if (!player_isActive(curr)){
          continue;
      }
      player_updateVisibleGrid(curr, game->masterGrid);
  }

}

static void displayAllPlayers(game_t* game){
    if (game == NULL){
        return;
    }

    player_t** players = game->players;
    char* display = "DISPLAY\n";
    int displayLen = strlen(display);
    for (int i = 0; i < game->numPlayer; ++i){
        player_t* player = players[i];
        if (!player_isActive(player)){
            continue;
        }
        
        char* gridString = grid_getDisplay(player_getVisibleGrid(player));
        int gridLen = strlen(gridString);
        int length = displayLen + gridLen;

        char* message = mem_calloc_assert(length + 1, sizeof(char), "Could not allocate memory for display grid of each player.\n");
        snprintf(message, length, "DISPLAY\n%s", gridString);
        player_sendMessage(player, message);

        mem_free(gridString);
        free(message);
    }
}

// A helper function that returns the result string
static char* get_result(game_t* game){
    if (game == NULL){
        return NULL;
    }

    int lineLength = MaxNameLength + 20;

    // need to malloc to return from function
    char* gameOverMessage = mem_calloc_assert(message_MaxBytes, sizeof(char), "out of memory\n");
    sprintf(gameOverMessage, "QUIT GAME OVER:\n");

    for(int i = 0; i < game->numPlayer; ++i){
        player_t* player = game->players[i];
        char line[lineLength];

        snprintf(line, lineLength, "%c %10d %s\n", player_getLetter(player), player_getGold(player), player_getName(player));
        strncat(gameOverMessage, line, lineLength);
    }

    return gameOverMessage;
}

// to delete everything in the game that was initialized before. Check game.h for more information. 
void game_over(game_t* game){

    // get the result of the game
    char* result = get_result(game);

    /************* delete players ************/
    // to delete each player one by one
    // sending each one the result before deleting

    for (int i = 0; i < game->numPlayer; i++){
        player_sendMessage(game->players[i], result);
        player_delete(game->players[i]);
    }

    free(result);


    // and then free the array of players too
    mem_free(game->players);

    // delete spectator
    if (game->spectator != NULL){
      spectator_delete(game->spectator);
    }
    // delete grid
    grid_delete(game->masterGrid);
    // free game structure
    mem_free(game);
}

/****************** findPlayerByCoords ********************
 *
 * returns the player at the given coordinates
 *
 */
static player_t*
findPlayerByCoords(game_t* game, const int x, const int y)
{
  if (game == NULL){
    return NULL;
  }

  char letter = grid_charAt(game->masterGrid, x, y);
  if (letter == '\0'){
    return NULL;
  }

  player_t** players = game->players;
  for (int i = 0; i < game->numPlayer; ++i) {
    player_t* player = players[i];
    if (player_getLetter(player) == letter && player_isActive(player)){
      return player;
    }
  }

  return NULL;
}

