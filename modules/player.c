/*
 *  player.c file. Check player.h for more information.
 * 
 * A 'player' is a struct to represent a player in a 'nuggets' game.
 * The struct keeps track of all information that defines a player 
 * Team: Torpedos, COSC 50, Febuary 2024
 * 
 * Author: Tayeb Mohammadi
 * 
 */

// Importing libraries 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "player.h"
#include "log.h"
#include "grid.h"
#include "mem.h"



/****************** player_new ****************************
 *
 * see player.h for description and usage
 *
 */
player_t*
player_new (addr_t* address, int x, int y, char* name, char character )
{
  if (address == NULL || name == NULL){
    return NULL;
  }

  player_t* player = mem_malloc_assert(sizeof(player_t), "Failed to allocte memory for player");

  int len = strlen(name);

  player->name = mem_malloc_assert(len * sizeof(char), "Failed to alocate memory for name of the player");
  strncpy(player->name, name, len); // make a copy of the passed in string

  player->character = character;
  player->x = x;
  player->y = y;
  player->address= address;
  player->gold = 0; // start off a new player with 0 gold

  return player;
}

// to delete the player. Check player.h for more information 
void player_delete(player_t* player){

    if(player != NULL){// to make sure that player has already initialized before deleting it
        mem_free(player->name); // free the name
        grid_delete(player->visibleGrid);
        mem_free(player); // free the player
    }
}

// to get the x cordinates of the player. Check player.h for more information 
int player_getX(const player_t* player){
    if(player == NULL){
        flog_v(stderr, "Cannot get x coordinate of null player.\n");
        return -3;
    }
    return player->x;
}

// to get the y cordinates of the player. Check player.h for more information 
int player_getY(const player_t* player){
    if(player == NULL){
        flog_v(stderr, "Cannot get y coordinate of null player.\n");
        return -2;
    }
    return player->y;
}

// to get the visible grid of the player. Check player.h for more information 
grid_t* player_getVisibleGrid(const player_t* player){
    if(player == NULL){
        flog_v(stderr, "Cannot get visible grid of null player.\n");
        return NULL;
    }
    return player->visibleGrid;
}

// to get the total gold claimed by the player. Check player.h for more information 
int player_getGold(const player_t* player){
    if(player == NULL){
        flog_v(stderr, "Cannot get gold number of null player.\n");
        return -1;
    }
    return player->gold;

}

// to get the name of the player. Check player.h for more information 
char* player_getName(const player_t* player){
    if(player == NULL){
        flog_v(stderr, "Cannot get name of null player.\n");
        return NULL;
    }
    return player->name;
}

/****************** player_getChar ****************************
 *
 * see player.h for description and usage
 *
 */
char
player_getChar(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get letter of null player.\n");
        return -1;
    }
    return player->character;
}

/****************** player_getAddress ****************************
 *
 * see player.h for description and usage
 *
 */
addr_t*
player_getAddress(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get letter of null player.\n");
        return NULL; // what to return when player is null ------------------------------------------ do we even need this?---------
    }
    return player->address;
}

// to move the player in x direction. Check player.h for more information 
void player_setX(player_t* player, int x){
    if(player == NULL){
        flog_v(stderr, "Cannot set x coordinates of null player.\n");
        return;
    }
    player->x += x;
}
 // to move the player in y direction. Check player.h for more information  
void player_setY(player_t* player, int y){
    if(player == NULL){
        flog_v(stderr, "Cannot set y coordinates of null player.\n");
        return;
    }
    player->y += y;
}

// to move the player in diagonally. Check player.h for more information 
void player_move(player_t* player, int x, int y){
    if(player == NULL){
        flog_v(stderr, "Cannot move null player.\n");
        return;
    }
    player->x += x;
    player->y += y;
}

// to add the new amount of gold to player's gold. Check player.h for more information 
void player_setGold(player_t* player, int gold){
    if(player == NULL){
        flog_v(stderr, "Cannot move null player.\n");
        return;
    }
    player->gold += gold;

}
// to set a new visible for the player. Check player.h for more information 
void player_setVisibleGrid(player_t* player, grid_t* visibleGrid){
    if(player == NULL){
        flog_v(stderr, "Cannot set grid for null player.\n");
        return;
    }

    if (player->visibleGrid != NULL){
      grid_delete(player->visibleGrid);
    }

    player->visibleGrid = visibleGrid;
}

// to send message to a player. Check player.h for more information 
void player_sendMessage(player_t* player, char* message){
    if(player == NULL){
        flog_v(stderr, "Cannot send message for null player.\n");
        return;
    }

    message_send(*(player->address), message);
}
