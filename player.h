/*
 * player.h- header file for player.c file
 * 
 * A 'player' is a struct to represent a player in a 'nuggets' game.
 * The struct keeps track of all information that defines a player 
 *
 * Paul Cherian, COSC 50, Febuary 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "grid.h"

/************* global types *************/
typedef struct player {
  int x;
  int y;
  grid_t* visibleGrid;
  int gold;
  char* name;
  char* letter; 
  addr_t address; // this should be the address not the pointer to address----------------------------
} player_t;


/************* player_new *************/
/* Creates a new player struct
 * Caller provides: 
 *  (x,y) coordaintes, name and address of client who is using the player
 * We do: 
 *  Initialize a new player_t* data struct with the parameters
 * We return:
 *  The initialized player, NULL if any failure
 * Notes: the caller should free memory for name and the player structure 
 *  when done.
 *
*/
player_t* player_new (grid_t* playerGrid, addr_t address, int x, int y, char* name, char letter );


/************* player_delete *************/
/* Deletes a player struct
 * Caller provides: 
 *  the player to delete
 * We do: 
 *  Delete the player, freeing all associated structs and their memory
 * We return:
 *  void
 *
*/
void player_delete(player_t* player);

/************* player_getX *************/
/* 
 * Get the x-coordinate of the player
 * Caller provides: 
 *  A pointer to the player from whom to get the x-coordinate.
 * We do: 
 *  Retrieve the x-coordinate of the player.
 * We return: 
 *  X-coordinate of the player as an int.
 */
int player_getX(const player_t* player);

/************* player_getY *************/
/* 
 * Get the y-coordinate of the player
 * Caller provides: 
 *  A pointer to the player from whom to get the y-coordinate.
 * We do: 
 *  Retrieve the y-coordinate of the player.
 * We return: 
 *  The y-coordinate of the player as an int.
 */
int player_getY(const player_t* player);

/************* player_getVisibleGrid *************/
/* 
 * Get the visible grid for the player
 * Caller provides: 
 *  A pointer to the player from whom to get the visible grid.
 * We do: 
 *  Retrieve the pointer to the visible grid of the  player.
 * We return: 
 *  A pointer to the visibleGrid struct.
 */
grid_t* player_getVisibleGrid(const player_t* player);

/************* player_getGold *************/
/* 
 * Get the gold amount of the player
 * Caller provides: 
 *  A pointer to the player from whom to get the gold amount.
 * We do: 
 *  Retrieve the gold amount of the player.
 * We return: 
 *  The gold amount of the player as an int.
 */
int player_getGold(const player_t* player);

/************* player_getName *************/
/* 
 * Get the name of the player
 * Caller provides: 
 *  A pointer to the player from whom to get the name.
 * We do: 
 *  Retrieve the name of the player.
 * We return: 
 *  A pointer to a string containing the name of the player.
 */
char* player_getName(const player_t* player);

/************* player_getLetter *************/
/* 
 * Get the letter of the player
 * Caller provides: 
 *  A pointer to the player from whom to get the letter.
 * We do: 
 *  Retrieve the letter of the player.
 * We return: 
 *  A pointer to a string containing the letter of the player.
 */
char player_getLetter(const player_t* player);

/************* player_getAddress *************/
/* 
 * Get the address of the player
 * Caller provides: 
 *  A pointer to the player from whom to get the address.
 * We do: 
 *  Retrieve the address of the  player.
 * We return: 
 *  The address of the player as an int.
 */
addr_t player_getAddress(const player_t* player);

/************* player_setX *************/
/* 
 * Set the x-coordinate of the player
 * Caller provides: 
 *  A pointer to the player and the new x-coordinate.
 * We do: 
 *  Set the x-coordinate of the player to the new value.
 * We return: 
 *  none
 */
void player_setX(player_t* player, int x);

/************* player_setY *************/
/* 
 * Set the y-coordinate of the player
 * Caller provides: 
 *  A pointer to the player and the new y-coordinate.
 * We do: 
 *  Set the y-coordinate of the player to the new value.
 * We return: 
 *  none
 */
void player_setY(player_t* player, int y);


/************* player_move *************/
/* 
 * Set the x and y coordinates of the player
 * Caller provides: 
 *  A pointer to the player and new  x and y coordinates.
 * We do: 
 *  Move the player diagonnaly x and y position
 * We return: 
 *  none
 */


void player_move(player_t* player, int x, int y);


/************* player_setGold *************/
/* 
 * Set the gold amount for the player
 * Caller provides: 
 *  A pointer to the player and the gold amount.
 * We do: 
 *  Set the gold amount of the player to the new amount.
 * We return: 
 *  none
 */
void player_setGold(player_t* player, int gold);

/************* player_setVisibleGrid *************/
/* 
 * Set the visible grid for the player
 * Caller provides: 
 *  A pointer to the player and a pointer to the new grid.
 * We do: 
 *  Set the visible grid of the player to the grid.
 * We return: 
 *  none
 */
void player_setVisibleGrid(player_t* player, grid_t* visibleGrid);


/************* player_sendMessage *************/
/* 
 * Send message to a player
 * Caller provides: 
 *  A pointer to the player and a pointer to message string
 * We do: 
 *  Send the message to player
 * We return: 
 *  none
 */

void player_sendMessage(player_t* player, char* message);