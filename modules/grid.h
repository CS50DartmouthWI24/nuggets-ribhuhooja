/*
 * grid - a module to implement grid functionality
 * for the cs50 nuggets program
 *
 * Ribhu Hooja, February 2024
 */

#ifndef __GRID_H
#define __GRID_H

#include <stdbool.h>
#include "counters.h"
#include "player.h"

/****************** global types *************************/
typedef struct grid grid_t;


/****************** functions ****************************/

/****************** grid_fromMap **************************
 *
 * creates a new grid
 *
 * Caller provides:
 *  valid file pointer to mapfile open for reading
 * We return:
 *  A new, valid heap allocated grid_*
 *  NULL if error
 * Caller is responsible for:
 *  Later calling grid_delete on the returned grid
 */
grid_t* grid_fromMap(FILE* mapFile);

/****************** grid_delete ***************************
 *
 * deletes a grid
 *
 * Caller provides:
 *  valid pointer to a grid
 * We do:
 *  delete the grid and free all its associated memory
 * Caller is responsible for:
 *  never using that pointer again
 */
void grid_delete(grid_t* grid);

/****************** grid_charAt ***************************
 *
 * Returns the character at the given (x,y) coordinate
 *
 * Caller provides:
 *  valid grid pointer
 *  x coordinate in [0, ncols - 1]
 *  y coordinate in [0, nrows - 1]
 * We return:
 *  null character '\0' if error
 *  the character at that (x,y) coordinate
 * Notes:   
 *  The character for a player coordinate will depend on which grid
 *  is querier. The base grid will return the player letter, and the
 *  player's own map will return an @.
 */
char grid_charAt(grid_t* grid, int x, int y);

/****************** grid_goldAt ***************************
 *
 * Returns the amount of gold at the given (x,y) coordinate
 *
 * Caller provides:
 *  valid grid pointer
 *  x coordinate in [0, ncols - 1]
 *  y coordinate in [0, nrows - 1]
 * We return:
 *  0 if error or if no gold at that coordinate
 *  the amount of gold  at that (x,y) coordinate
 * Notes:   
 *  This should ONLY be called on the base grid, not the 'visible' grids
 *  stored inside a player struct, because the only purpose of that grid is
 *  displaying. Calling it on a 'player' grid is undefined behavior. That
 *  can lead to issues such as a '*' block returning 0 gold nuggets.
 */
int grid_goldAt(grid_t* grid, const int x, const int y);

/****************** grid_generateVisibleGrid **************
 *
 * Generates the grid visible to a given player
 *
 * Caller provides:
 *  valid pointer to a grid
 *  valid pointer to a 'valid' player - player is in the game, at valid 
 *  coordinates, etc.
 * We return:
 *  A pointer to the grid visible to the player
 * We do:
 *  We update the grid stored inside the player for doing more
 *  visibility checks later
 * Notes:
 *  The caller SHOULD NOT call grid_delete on the returned grid because
 *  it is stored inside the player, and will be freed by player_delete
 *
 *  NONE of the other grid function in this module should be called on the grid
 *  returned by this, as this grid is for display purposes only
 *
 *  Only the following functions are defined on this grid:
 *  - getDisplay
 *  - toMap
 *  - charAt
 *
 */
grid_t* grid_generateVisibleGrid(grid_t* grid, player_t* player);

/****************** grid_addPlayer ************************
 *
 * Adds a player to the grid
 *
 * Caller provides:
 *  valid pointer to a grid
 *  valid character representation of the player (eg A, B, etc.)
 *  (Do not provide '@', or whatever the player is supposed to see themselves as;
 *  provide what OTHER players are supposed to see the player as)
 * We do:
 *  update the grid to hold a representation of the player at that point
 * We return:
 *  true if the operation succeeded
 *  false if there was an error or if location was invalid
 * Note:
 *  Does NOT allow adding player to a location that has gold on it.
 *
 *  This very directly puts a player at that location, only checking 
 *  if it is a valid. It does not check for the limit of players
 *  because that is a check that GAME needs to do, not grid. Grid
 *  does not know about the player limit. It also does not check if
 *  the provided character is '@', or whatever the selected player 
 *  character is. It is upto game to check that the character being 
 *  given here is valid.
 */
bool grid_addPlayer(grid_t* grid, const int x, const int y, char playerChar);

/****************** grid_movePlayer ***********************
 *
 * Moves a player
 *
 * Caller provides:
 *  valid pointer to grid and player
 *  valid move directions - both x_move and y_move must be 0, 1 or -1
 * We do:
 *  update the grid to represent the player having moved.
 * We return:
 *  -1 if the operation failed
 *  the amount of gold collected if the operation succeeded
 */
int grid_movePlayer(grid_t* grid, player_t* player, int x_move, int y_move);

/****************** grid_collectGold **********************
 *
 * Makes the player collect the gold at its location
 *
 * Caller provides:
 *  valid pointer to grid and player
 * We do:
 *  Add the gold to the player
 * We do NOT:
 *  Update the grid character, because the player moved onto that character
 * We return:
 *  The amount of gold collected
 * Note:
 *  The responsibility for updating game state and sending messages is NOT
 *  handled by this module
 */
int grid_collectGold(grid_t* grid, player_t* player);

/****************** grid_getDisplay **************************
 *
 * returns the grid string for display/debug purposes
 *
 * Caller provdes:
 *  valid pointer to a grid
 * We return:
 *  the grid string within the grid
 * Caller is responsible for:
 *  NOT freeing the returned string. The string is freed when grid_delete is
 *  called.
 */
char* grid_getDisplay(grid_t* grid);

/****************** grid_toMap ****************************
 *
 * prints the grid string to a file
 *
 * Caller provides:
 *  valid pointer to a grid 
 *  valid file open for writing
 * We do:
 *  print the grid to that file in string format
 */
void grid_toMap(grid_t* grid, FILE* fp);

#endif    // __GRID_H
