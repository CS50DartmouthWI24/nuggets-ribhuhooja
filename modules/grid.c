/*
 * grid.c - a file implementing the grid module for 
 * the cs50 nuggets game
 *
 * usage and description is given in grid.h
 *
 * Ribhu Hooja, February 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mem.h"
#include "counters.h"
#include "hashtable.h"
#include "player.h"
#include "grid.h"
#include "mapchars.h"

/****************** types ********************************/
typedef struct grid_t {
  char* string;         // the string representation of the grid
  int numrows;          // number of rows
  int numcols;          // number of columns
  counters_t* nuggets;  // number of nuggets at a location, keyed by string index
  hashtable_t* playersStandingOn;   // what character each player is standing on
}

/****************** file-local global variables **********/
/* none */

/****************** file-local global constants **********/
const int initGridStringSize = 2000; // the initial string size allocated when reading
                                     // a grid from a file

/****************** local function prototypes ************/
static inline int indexOf(const int x, const int y, const int numcols);
static inline bool isValidCoordinate(const int x, const int y, const int numrows
                                                               const int numcols);
static bool isVisible(grid_t* grid, const int px, const int py, const int x, 
                                                                const int y);
static char getPlayerStandingOn(grid_t* grid, player_t* player);
static void setPlayerStandingOn(grid_t* grid, player_t* player, const char newChar);

/****************** global function prototypes ***********/
/* see grid.h for description and usage */

/****************** grid_fromMap **************************
 *
 * see grid.h for usage and description
 *
 * the code assumes that the given map is valid, properly formatted, etc.
 *
 */
grid_t* grid_fromMap(FILE* mapFile){
  if (mapFile == NULL){
    return NULL;
  }

  char* string = calloc(initGridStringSize, sizeof(char));
  mem_assert(string, "out of memory\n");
  int stringBufsize = initGridStringSize;

  int numcols = 0;
  char readChar;
  while ((readChar = fgetc(mapFile)) != '\n'){
    if (readChar == EOF){
      free(string);
      return NULL;
    }

    // expand the string buffer if needed
    if (numcols > initGridStringSize){
      stringBufsize *= 2;
      string = realloc(string, stringBufsize * sizeof(char));
      mem_assert(string, "out of memory\n");
    }
    string[numcols] = readChar;
    ++numCols;
  }
  // at this point readChar is a newline, and numcols is the 
  // number of characters read before reading a newline
  string[numcols] = '\n';

  int numrows = 1;  // the number of rows is equal to the number of newlines read
  int i = numcols + 1;
  while ((readChar = fgetc(mapFile)) != EOF){
    if (readChar == '\n'){
      ++numrows;
    }

    // expand the string buffer if needed
    if (i > initGridStringSize){
      stringBufsize *= 2;
      string = realloc(string, stringBufsize * sizeof(char));
      mem_assert(string, "out of memory\n");
    }
    string[i] = readchar;
    ++i;
  }

  // do some size checks; i should be numrows * (numcols + 1)
  if (i != numrows * (numcols + 1)){
    fprintf(stderr, "inconsitent map read\n");
    free(string);
    return NULL;
  }

  // now properly size the array, which has read i characters
  // add one for the null character
  string = realloc(string, (i + 1) * sizeof(char));
  mem_assert(string, "out of memory\n");
  string[i] = '\0';

  grid_t* new = malloc(sizeof(grid_t));
  mem_assert(new, "out of memory; could not create grid from mapfile\n");

  new->string = string;
  new->numrows = numrows;
  new->numcols = numcols;

  counters_t* ctrs = counters_new();
  mem_assert(ctrs, "out of memory; could not allocate space for nuggets counter\n");

  hashtable_t* ht = hashtable_new();
  mem_assert(ht, "out of memory; could not allocate space for player hashtable\n");

  new->nuggets = ctrs;
  new->playersStandingOn = ht;
  
  return new;
}

/****************** grid_delete ***************************
 *
 * see grid.h for usage and description
 *
 */
void
grid_delete(grid_t* grid)
{
  if (grid == NULL){
    return;
  }

  if (grid->string != NULL){
    free(grid->string);
  }

  if (grid->nuggets != NULL){
    counters_delete(grid->nuggets);
  }

  if (grid->playersStandingOn != NULL){
    hashtable_delete(grid->playersStandingOn);
  }

  free(grid);
}


/****************** grid_charAt ***************************
 *
 * see grid.h for usage and description
 *
 */
char
grid_charAt(grid_t* grid, int x, int y)
{
  // we do a lot of validity checking in this function
  // to avoid bad-indexing an array
  if (grid == NULL || grid->string == NULL || x < 0 || y < 0){
    return '\0';
  }

  int numrows = grid->numrows;
  int numcols = grid->numcols;
  if (x >= numcols || y >= numrows){
    return '\0';
  }

  return grid->string[indexOf(x, y, numcols)];
}

/****************** grid_goldAt ***************************
 *
 * see grid.h for usage and description
 *
 */
int
grid_goldAt(grid_t* grid, const int x, const int y)
{
  if (grid == NULL || grid->nuggets == NULL){
    return 0;
  }

  return counters_get(nuggets, indexOf(x, y, grid->numcols));
}

/****************** grid_generateVisibleGrid **************
 *
 * see grid.h for usage and description
 *
 */
grid_t*
grid_generateVisibleGrid(grid_t* grid, player_t* player)
{
  if (grid == NULL || player == NULL){
    return NULL;
  }

  int px = player_getX(player);
  int py = player_getY(player);
  int numrows = grid->numrows;
  int numcols = grid->numcols;

  if (!isValidCoordinate(px, py, numcols)){
      return NULL;
  }

  // this means that the visibility check has never been performed before
  // start off the player with all map spots blank i.e. solid rock
  if (player->grid == NULL){
    grid_t* new = malloc(sizeof(grid_t));
    mem_assert(new, "out of memory; could not make new grid for visibility\n");

    new->numrows = numrows;
    new->numcols = numcols;
    new->nuggets = NULL;   // NUGGETS INFO MUST NOT BE ACCESSED FROM PLAYER GRIDS

    int len = numrows * (numcols + 1);
    char* newString = calloc(len, sizeof(char));
    for (int i = 0; i < len; ++i){
      if (i % numcols == 0){
        newString[i] = '\n';
      } else {
        newString[i] = mapchars_solidRock;
      }
    }
    new->string = newString;
  }

  grid_t* pgrid = player->grid;

  for (int x = 0; x < numcols; ++x){
    for (int y = 0; y < numrows; ++y){
      //TODO: add history checking
      if (isVisible(grid, px, py, x, y)){
        pgrid->string[indexOf(x, y, numcols)] = grid_charAt(grid, x, y);
      } else {
        pgrid->string[indexOf(x, y, numcols)] = mapchars_solidRock;
      }
    }
  }

  pgrid->string[indexOf(px, py, numcols)] = mapchars_player;

  return pgrid;
}

/****************** grid_addPlayer ************************
 *
 * see grid.h for usage and description
 *
 */
bool
grid_addPlayer(grid_t* grid, const int x, const int y, char playerChar)
{
  if (grid == NULL){
    return false;
  }

  if (!(isValidCoordinate(x, y, grid->numrows, grid->numcols))){
    return false;
  }

  // we can only put a player at an empty room spot
  if (grid_CharAt(grid, x, y) != mapchars_roomSpot){
    return false;
  }

  grid->string[indexOf(x, y, grid->numcols)] = playerChar;
  setPlayerStandingOn(grid, player, mapchars_roomSpot);
}

/****************** grid_movePlayer ***********************
 *
 * see grid.h for usage and description
 *
 */
int
grid_movePlayer(grid_t* grid, player_t* player, int x_move, int y_move)
{
  if (grid == NULL || grid->string == NULL || player == NULL){
    return -1;
  }

  if (x_move > 1 || x_move < -1 || y_move > 1 || y_move < -1){
    return -1;
  }

  int x = player_getX(player);
  int y = player_getY(player);
  int x_new = x + x_move;
  int y_new = y + y_move;

  int numcols = grid->numcols;  // this is needed multiple times

  if (!isValidCoordinate(x_new, y_new, grid->numrows, numcols)){
    return -1;
  }

  // player can only move to a room spot or a passage spot, or a gold spot
  // or there can be another player there, in which case we swap them
  char moveSpot = grid_charAt(grid, x_new, y_new);
  if (!(moveSpot == mapchars_roomSpot 
        || moveSpot == mapchars_passageSpot
        || moveSpot == mapchars_gold
        || moveSpot == mapchars_player)){
    return -1;
  }

  //TODO: add swap code
  if (moveSpot == mapchars_player){
    return -1;
  }

  player_setX(player, x_new);
  player_setY(player, Y_new);

  int gold = 0;
  if (moveSpot == mapchars_gold){
    gold = grid_collectGold(grid, player);
  }

  // update string visuals - TODO: doesn't do swapping yet
  int oldIndex = indexOf(x, y, numcols);
  int newIndex = indexOf(x_new, y_new, numcols);
  char* string = grid->string;

  string[oldIndex] = getPlayerStandingOn(grid, player);
  setPlayerStandingOn(grid, player, string[newIndex]);
  string[newIndex] = player_getChar(player);

  return gold;
}

/****************** grid_removePlayer *********************
 *
 * see grid.h for usage and description
 *
 */
bool
grid_removePlayer(grid_t* grid, player_t* player)
{
  if (grid == NULL || player == NULL){
    return false;
  }

  int px = player_getX(player);
  int py = player_getY(player);

  if (!isValidCoordinate(px, py, grid->numrows, grid->numcols)){
    return false;
  }

  grid->string[indexOf(px, py, numcols)] = getPlayerStandingOn(grid, 
                                                              player_getChar(player));

  return true;
}


/****************** grid_collectGold **********************
 *
 * see grid.h for usage and description
 *
 */
int
grid_collectGold(grid_t* grid, player_t* player)
{
  if (grid == NULL ||grid->nuggets == NULL || player == NULL){
    return 0;
  }

  int x = player_getX(player);
  int y = player_getY(player);

  if (!isValidCoordinate(x, y, grid->numrows, grid->numcols)){
    return 0;
  }

  int gold = grid_goldAt(grid, x, y);
  player_setGold(player, player_getGold(player) + gold);
  counters_set(grid->nuggets, indexOf(x, y, grid->numcols), 0);
}

/****************** grid_getDisplay **************************
 *
 * see grid.h for usage and description
 *
 */
char*
grid_getDisplay(grid_t* grid)
{
  if (grid == NULL){
    return NULL;
  }

  return grid->string;
}

/****************** grid_toMap ****************************
 *
 * see grid.h for usage and description
 *
 */
void
grid_toMap(grid_t* grid, FILE* fp)
{
  if (grid == NULL || fp == NULL){
    return;
  }

  char* toPrint = grid_getDisplay(grid);
  if (toPrint == NULL){
    return;
  }

  fputs(toPrint, fp);
}


/****************** indexOf *******************************
 *
 * gives the index of the (x,y) coordinate
 *
 * does NO checking for whether the arguments are valid
 *
 */
static inline int
indexOf(const int x, const int y, const int numcols)
{
  // each line of the string contains numcols + 1 characters
  // because of the newline. So the index of the first character
  // on line 0 is 0, last char is numcols, first char on line 1
  // is numcols + 1. => first char of line n is n*numcols + 1
  // (x,y) has index numcols * y + x
  
  return numcols * y + x;
}

/****************** isValidCoordinate *********************
 *
 * returns whether this coordinate is valid
 * this is important when indexing into the grid string to avoid
 * a bad read
 *
 */
static inline bool
isValidCoordinate(const int x, const int y, const int numrows, const int numcols)
{
  return x >= 0 && y >= 0 && x < numcols && y < numrows;
}

/****************** isVisible *****************************
 *
 * returns whether the point (x,y) is visible from the point (px, py)
 *
 * TODO - fix this
 */
static bool
isVisible(grid_t* grid, const int px, const int py, const int x, const int y)
{
  return true;
}

/****************** getPlayerStandingOn *******************
 *
 * returns the character that the player is standing on
 *
 */
static void
getPlayerStandingOn(grid_t* grid, player_t* player)
{
  if (grid == NULL || player == NULL){
    return mapchars_roomSpot;
  }

  char* charString = calloc(2, sizeof(char));
  mem_assert(charString, "out of memory\n");

  charString[0] = player_getChar(player);
  charString[1] = '\0';

  char result = hashtable-get(grid->playersStandingOn, charString);
  free(charString);

  return result;
}

/****************** setPlayerStandingOn *******************
 *
 * updates the grid's bookkeeping of what the player is standing on
 *
 */
static void
setPlayerStandingOn(grid_t* grid, player_t* player, const char newChar)
{
  if (grid == NULL || player == NULL){
    return;
  }

  char* charString = calloc(2, sizeof(char));
  mem_assert(charString, "out of memory\n");

  charString[0] = player_getChar(player);
  charString[1] = '\0';

  hashtable_insert(grid->playersStandingOn, charString, newChar);
  free(charString);
}


