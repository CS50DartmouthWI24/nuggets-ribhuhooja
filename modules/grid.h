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

/****************** grid_numrows **************************
 *
 * Caller provides:
 *  valid pointer to a grid
 * We return:
 *  the number of rows in the grid
 *  0 if error
 */
int grid_numrows(grid_t* grid);

/****************** grid_numcols **************************
 *
 * Caller provides:
 *  valid pointer to a grid
 * We return:
 *  the number of columns in the grid
 *  0 if error
 */
int grid_numcols(grid_t* grid);

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
 *  is queried. The base grid will return the player letter, and the
 *  player's own map will return an @.
 */
char grid_charAt(grid_t* grid, const int x, const int y);

/****************** grid_baseCharAt ***************************
 *
 * Returns the BASE character at the given (x,y) coordinate
 * i.e. the character in the base map, before nuggets and players
 * were added
 *
 * Caller provides:
 *  valid grid pointer
 *  x coordinate in [0, ncols - 1]
 *  y coordinate in [0, nrows - 1]
 * We return:
 *  null character '\0' if error
 *  the base character at that (x,y) coordinate
 * Notes:   
 *  The character for a player coordinate will depend on which grid
 *  is queried. The base grid will return the player letter, and the
 *  player's own map will return an @.
 */
char grid_baseCharAt(grid_t* grid, const int x, const int y);

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

/****************** grid_nuggetsPopulate ******************
 *
 * Populate the grid with nuggets
 *
 * Caller provides:
 *  valid pointer to a grid
 *  valid values for minNumPiles, maxNumPiles and goldTotal
 *  NOTES: maxNumPiles MUST BE <= goldTotal
 *  maxNumPiles must also be <= the number of room spots in the grid
 * We return:
 *  true if the operation was successful
 *  false if the operation failed
 * We do:
 *  We select an integer randomly from [minNumPiles, maxNumPiles]
 *  We create this number of piles on the grid
 *  For each gold nugget, we randomly select one of these piles and put it there
 * We do NOT:
 *  call srand(). srand() must have been called before this function is called.
 * Notes:
 *  If this is called twice on the same grid it will add more nuggets to the grid,
 *  though none of the positions will overlap
 */
bool grid_nuggetsPopulate(grid_t* grid, const int minNumPiles, const int maxNumPiles,
                                                               const int goldTotal);

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
 *
 */
grid_t* grid_generateVisibleGrid(grid_t* grid, grid_t* currentlyVisibleGrid,
                                               const int px,
                                               const int py);

/****************** grid_findRandomSpawnPosition **********
 *
 * finds a random spot where a player can be added
 *
 * Caller provides:
 *  valid pointer to a grid
 *  valid pointers to the x and y coordinates
 * We do:
 *  Find a random empty room spot 
 *  put the coordinates of the spot at the addresses given as parameters
 */
bool  grid_findRandomSpawnPosition(grid_t* grid, int* pX, int* pY);

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
bool grid_addPlayer(grid_t* grid, const int x, const int y, const char playerChar);

/****************** grid_movePlayer ***********************
 *
 * Moves a player
 *
 * Caller provides:
 *  valid pointer to grid, valid player coordinates inside grid
 *  valid move directions - both x_move and y_move must be 0, 1 or -1
 * We do:
 *  update the grid to represent the player having moved.
 * We do NOT:
 *  update the player's internal state. That is handled by the game
 * We return:
 *  -1 if the operation failed
 *  -2 if there is another player at the location; call swapPlayers
 *  the amount of gold collected if the operation succeeded
 */
int grid_movePlayer(grid_t* grid, const int px, const int py, const int x_move,
                                                              const int y_move);

/****************** grid_swapPlayers **********************
 *
 * swaps two players on the grid
 *
 * Caller provides:
 *  valid pointer to grid
 *  calid coordiantes on the grid
 * We do:
 *  We swap the two players on the grid
 *  (this does not update game state, only grid)
 */
void grid_swapPlayers(grid_t* grid, const int x1, const int y1, const int x2,
                                                                const int y2);
/****************** grid_removePlayer *********************
 *
 * removes a player from the grid
 *
 * Caller provides:
 *  valid pointer to grid
 *  valid coordinates to player in grid
 * We do:
 *  remove the player from the grid
 * We return:
 *  true if the operation succeeded
 *  false if the operation failed
 */
bool grid_removePlayer(grid_t* grid, const char playerChar, const int px,
                                                            const int py);

/****************** grid_collectGold **********************
 *
 * Makes the player collect the gold at its location
 *
 * Caller provides:
 *  valid pointer to grid and valid coordiantes inside grid
 * We do NOT:
 *  Update the amount of gold stored inside the player. That must be done by game.
 * We return:
 *  The amount of gold collected
 * Note:
 *  The responsibility for updating game state and sending messages is NOT
 *  handled by this module
 */
int grid_collectGold(grid_t* grid, const int px, const int py);

/****************** grid_getDisplay **************************
 *
 * returns the grid string for display/debug purposes
 *
 * Caller provdes:
 *  valid pointer to a grid
 * We return:
 *  a copy of the grid string within the grid
 * Caller is responsible for:
 *  freeing the returned string.
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
