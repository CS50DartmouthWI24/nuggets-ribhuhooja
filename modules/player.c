/*
 *  player.c file. Check player.h for more information.
 * 
 * A 'player' is a struct to represent a player in a 'nuggets' game.
 * The struct keeps track of all information that defines a player 
 * Team: Torpedos, COSC 50, Febuary 2024
 * 
 * Author: Tayeb Mohammadi, February 2024
 * Modified: Ribhu Hooja, February 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"
#include "player.h"
#include "log.h"
#include "grid.h"
#include "mem.h"    


/******************* types *******************************/
typedef struct player {
  int x;                    // x-coordinate of the player
  int y;                    // y-coordinate of the player
  grid_t* visibleGrid;      // the grid that the player can see
  int gold;                 // the gold collected by the player
  char* name;               // the name of the player
  bool isActive;            // whether the player is active
  char letter;           // the character representation of the player on the map
  addr_t* address;          // the address of the player client, for sending messages
} player_t;

/****************** player_new ****************************
 *
 * see player.h for description and usage
 *
 */
player_t*
player_new (addr_t* address, int x, int y, char* name, char letter )
{
  if (address == NULL || name == NULL){
    return NULL;
  }

  player_t* player = mem_malloc_assert(sizeof(player_t), "Failed to allocate memory for player");

  int len = strlen(name);

  player->name = mem_malloc_assert(len * sizeof(char), "Failed to allocate memory for name of the player");
  strncpy(player->name, name, len); // make a copy of the passed in string

  player->letter = letter;
  player->x = x;
  player->y = y;
  player->isActive = true;
  player->address= address;
  player->gold = 0; // start off a new player with 0 gold

  player->visibleGrid = NULL;

  return player;
}


/****************** player_delete *************************
 *
 * see player.h for description and usage
 *
 */
void
player_delete(player_t* player)
{
  if (player == NULL){
    return;
  }

  mem_free(player->name); 
  grid_delete(player->visibleGrid);
  mem_free(player); 

}

/****************** player_getX ****************************
 *
 * see player.h for description and usage
 *
 */
int
player_getX(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get x coordinate of null player.\n");
        return -1;
    }

    return player->x;
}

/****************** player_getY ****************************
 *
 * see player.h for description and usage
 *
 */
int
player_getY(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get y coordinate of null player.\n");
        return -1;
    }

    return player->y;
}

/****************** player_getY ****************************
 *
 * see player.h for description and usage
 *
 */
grid_t*
player_getVisibleGrid(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get visible grid of null player.\n");
        return NULL;
    }

    return player->visibleGrid;
}

/****************** player_getGold ****************************
 *
 * see player.h for description and usage
 *
 */
int
player_getGold(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get gold number of null player.\n");
        return 0;
    }

    return player->gold;

}

/****************** player_getName ****************************
 *
 * see player.h for description and usage
 *
 */
char*
player_getName(const player_t* player)
{
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
player_getLetter(const player_t* player)
{
    if(player == NULL){
        flog_v(stderr, "Cannot get letter of null player.\n");
        return '\0';
    }
    return player->letter;
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
        flog_v(stderr, "Cannot get address of null player.\n");
        return NULL; 
    }

    return player->address;
}

/****************** player_setX ****************************
 *
 * see player.h for description and usage
 *
 */
void
player_setX(player_t* player, int x)
{
    if(player == NULL){
        flog_v(stderr, "Cannot set x coordinates of null player.\n");
        return;
    }

    player->x = x;
}

/****************** player_setY ****************************
 *
 * see player.h for description and usage
 *
 */
void
player_setY(player_t* player, int y)
{
    if(player == NULL){
        flog_v(stderr, "Cannot set y coordinates of null player.\n");
        return;
    }

    player->y = y;
}

/****************** player_setGold ****************************
 *
 * see player.h for description and usage
 *
 */
void
player_setGold(player_t* player, int gold)
{
    if(player == NULL){
        flog_v(stderr, "Cannot set gold for null player.\n");
        return;
    }

    player->gold = gold;

}

/****************** player_setVisibleGrid ****************************
 *
 * see player.h for description and usage
 *
 */
void
player_setVisibleGrid(player_t* player, grid_t* visibleGrid)
{
    if(player == NULL){
        flog_v(stderr, "Cannot set grid for null player.\n");
        return;
    }

    if (player->visibleGrid != NULL){
      grid_delete(player->visibleGrid);
    }

    player->visibleGrid = visibleGrid;
}

/****************** player_sendMessage ****************************
 *
 * see player.h for description and usage
 *
 */
void
player_sendMessage(player_t* player, char* message)
{
    if(player == NULL || player->address == NULL){
        flog_v(stderr, "Cannot send message for null player or address.\n");
        return;
    }

    message_send(*(player->address), message);
}
