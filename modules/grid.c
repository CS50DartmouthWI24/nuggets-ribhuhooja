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
#include "grid.h"
#include "mem.h"
#include "counters.h"
#include "hashtable.h"
#include "mapchars.h"

/****************** types ********************************/
typedef struct grid {
  char* string;         // the string representation of the grid
  int numrows;          // number of rows
  int numcols;          // number of columns
  counters_t* nuggets;  // number of nuggets at a location, keyed by string index
  hashtable_t* playersStandingOn;   // what character each player is standing on
} grid_t;

/****************** file-local global variables **********/
/* none */

/****************** file-local global constants **********/
// the initial string size allocated when reading a grid from a file
static const int initGridStringSize = 2000; 
static const int numSlotsPlayerHt = 26;

/****************** local function prototypes ************/
static inline int indexOf(const int x, const int y, const int numcols);
static inline bool isValidCoordinate(const int x, const int y, const int numrows,
                                                               const int numcols);
static bool isVisible(grid_t* grid, const int px, const int py, const int x, 
                                                                const int y);
static char getPlayerStandingOn(grid_t* grid, const char playerChar);
static void setPlayerStandingOn(grid_t* grid, const char playerChar,
                                              const char newChar);
static void freeCharItemdelete(void* pChar);

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
    ++numcols;
  }
  // at this point readChar is a newline, and numcols is the 
  // number of characters read before reading a newline
  string[numcols] = '\n';

  // the number of rows is equal to the number of newlines read
  int numrows = 1;  
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
    string[i] = readChar;
    ++i;
  }

  // do some size checks; i should be numrows * (numcols + 1)
  if (i != numrows * (numcols + 1)){
    fprintf(stderr, "inconsistent map read:\n");
    fprintf(stderr, "Read %d rows and %d columns\n", numrows, numcols);
    fprintf(stderr, "Read %d characters\n", i);
    fprintf(stderr, "Expected rows * (columns + 1) = %d characters\n", 
                                                     numrows * (numcols + 1));
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

  hashtable_t* ht = hashtable_new(numSlotsPlayerHt);
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
    hashtable_delete(grid->playersStandingOn, freeCharItemdelete); 
  }

  free(grid);
}

/****************** grid_numrows **************************
 *
 * see grid.h for usage and description
 *
 */
int
grid_numrows(grid_t* grid)
{
  if (grid == NULL){
    return 0;
  }

  return grid->numrows;
}

/****************** grid_numcols **************************
 *
 * see grid.h for usage and description
 *
 */
int
grid_numcols(grid_t* grid)
{
  if (grid == NULL){
    return 0;
  }

  return grid->numcols;
}

/****************** grid_charAt ***************************
 *
 * see grid.h for usage and description
 *
 */
char
grid_charAt(grid_t* grid, const int x, const int y)
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

  return counters_get(grid->nuggets, indexOf(x, y, grid->numcols));
}

/****************** grid_nuggetsPopulate ******************
 *
 * see grid.h for usage and description
 *
 */
bool
grid_nuggetsPopulate(grid_t* grid, const int minNumPiles, const int maxNumPiles,
                                                          const int goldTotal)
{
  if (grid == NULL || minNumPiles <= 0 || maxNumPiles <= 0 || goldTotal <= 0){
    return false;
  }

  if (minNumPiles > maxNumPiles || maxNumPiles > goldTotal){
    return false;
  }

  // checking if there are enough room spots
  // also simultaneously get the length of the string
  char* string = grid->string;
  int numRoomSpots = 0;
  for (int i = 0; string[i] != '\0'; ++i){
    if (string[i] == mapchars_roomSpot){
      ++numRoomSpots;
    }
  }

  if (numRoomSpots < maxNumPiles){
    return false;
  }

  // generate number of piles
  int numPiles;
  numPiles = minNumPiles + (rand() % (maxNumPiles - minNumPiles + 1));

  // choose the spots for the piles, and put them in an array for
  // later populating with nuggets
  int spots[numPiles];
  counters_t* nuggets = grid->nuggets;
  int stringLength = (grid->numcols + 1) * grid->numrows;
  for (int i = 0; i < numPiles; ){ // only incrementing when correctly chosen spot
    // randomly get a spot
    // if it is a roomSpot, put a nugget there and increment the counter
    int chosenSpot = rand() % stringLength;
    if (string[chosenSpot] == mapchars_roomSpot){
      string[chosenSpot] = mapchars_gold;
      spots[i] = chosenSpot;
      counters_set(nuggets, chosenSpot, 0);
      ++i;
    }
  }

  // for each nugget, put it in one of the chosen piles
  for (int i = 0; i < goldTotal; ++i){
    int chosenIndex = rand() % numPiles;
    counters_add(nuggets, spots[chosenIndex]);
  }

  return true;
}

/****************** grid_generateVisibleGrid **************
 *
 * see grid.h for usage and description
 *
 */
grid_t*
grid_generateVisibleGrid(grid_t* grid, grid_t* currentlyVisibleGrid, const int px,
                                                                     const int py)
{
  if (grid == NULL){
    return NULL;
  }

  int numrows = grid->numrows;
  int numcols = grid->numcols;

  if (!isValidCoordinate(px, py, numrows, numcols)){
      return NULL;
  }

  // this means that the visibility check has never been performed before
  // start off the player with all map spots blank i.e. solid rock
  // if (player->grid == NULL){
  //   grid_t* new = malloc(sizeof(grid_t));
  //   mem_assert(new, "out of memory; could not make new grid for visibility\n");
 
  //   new->numrows = numrows;
  //   new->numcols = numcols;
  //   new->nuggets = NULL;   // NUGGETS INFO MUST NOT BE ACCESSED FROM PLAYER GRIDS
 
  //   int len = numrows * (numcols + 1);
  //   char* newString = calloc(len, sizeof(char));
  //   for (int i = 0; i < len; ++i){
  //     if (i % numcols == 0){
  //       newString[i] = '\n';
  //     } else {
  //       newString[i] = mapchars_solidRock;
  //     }
  //   }
  //   new->string = newString;
  // }

  for (int x = 0; x < numcols; ++x){
    for (int y = 0; y < numrows; ++y){
      //TODO: add history checking
      if (isVisible(grid, px, py, x, y)){
        currentlyVisibleGrid->string[indexOf(x, y, numcols)] = grid_charAt(grid, x, y);
      } else {
        currentlyVisibleGrid->string[indexOf(x, y, numcols)] = mapchars_solidRock;
      }
    }
  }

  currentlyVisibleGrid->string[indexOf(px, py, numcols)] = mapchars_player;

  return currentlyVisibleGrid;
}

/****************** grid_addPlayer ************************
 *
 * see grid.h for usage and description
 *
 */
bool
grid_addPlayer(grid_t* grid, const int x, const int y, const char playerChar)
{
  if (grid == NULL){
    return false;
  }

  if (!(isValidCoordinate(x, y, grid->numrows, grid->numcols))){
    return false;
  }

  // we can only put a player at an empty room spot
  if (grid_charAt(grid, x, y) != mapchars_roomSpot){
    return false;
  }

  grid->string[indexOf(x, y, grid->numcols)] = playerChar;
  setPlayerStandingOn(grid, playerChar, mapchars_roomSpot);

  return true;
}

/****************** grid_movePlayer ***********************
 *
 * see grid.h for usage and description
 *
 */
int
grid_movePlayer(grid_t* grid, const int px, const int py, const int x_move,
                                                          const int y_move)
{
  if (grid == NULL || grid->string == NULL){
    return -1;
  }

  if (x_move > 1 || x_move < -1 || y_move > 1 || y_move < -1){
    return -1;
  }

  int x_new = px + x_move;
  int y_new = py + y_move;
  

  int numcols = grid->numcols;  // this is needed multiple times

  if (!isValidCoordinate(x_new, y_new, grid->numrows, numcols)){
    return -1;
  }

  // player can only move to a room spot or a passage spot, or a gold spot
  // or there can be another player there, in which case we swap them
  // TODO: Currently does not support another player standing there
  char moveSpot = grid_charAt(grid, x_new, y_new);
  if (!(moveSpot == mapchars_roomSpot 
        || moveSpot == mapchars_passageSpot
        || moveSpot == mapchars_gold)){
    return -1;
  }

  //TODO: add swap code

  int gold = 0;
  if (moveSpot == mapchars_gold){
    gold = grid_collectGold(grid, px, py);
  }

  // update string visuals - TODO: doesn't do swapping yet
  int oldIndex = indexOf(px, py, numcols);
  int newIndex = indexOf(x_new, y_new, numcols);
  char* string = grid->string;
  char playerChar = string[oldIndex];

  string[oldIndex] = getPlayerStandingOn(grid, playerChar);
  setPlayerStandingOn(grid, playerChar, string[newIndex]);
  string[newIndex] = playerChar; 

  return gold;
}

/****************** grid_removePlayer *********************
 *
 * see grid.h for usage and description
 *
 */
bool
grid_removePlayer(grid_t* grid, const char playerChar, const int px, const int py)
{
  if (grid == NULL){
    return false;
  }

  if (!isValidCoordinate(px, py, grid->numrows, grid->numcols)){
    return false;
  }

  grid->string[indexOf(px, py, grid->numcols)] = getPlayerStandingOn(grid,
                                                                     playerChar);

  return true;
}


/****************** grid_collectGold **********************
 *
 * see grid.h for usage and description
 *
 */
int
grid_collectGold(grid_t* grid, const int px, const int py)
{
  if (grid == NULL ||grid->nuggets == NULL){
    return 0;
  }

  if (!isValidCoordinate(px, py, grid->numrows, grid->numcols)){
    return 0;
  }

  int gold = grid_goldAt(grid, px, py);
  counters_set(grid->nuggets, indexOf(px, py, grid->numcols), 0);

  return gold;
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
  // on line 0 is 0, last char is numcols - 1, first char on line 1
  // is numcols + 1. => first char of line n is n*(numcols + 1)
  // (x,y) has index (numcols + 1) * y + x
  
  return (numcols + 1) * y + x;
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
static char
getPlayerStandingOn(grid_t* grid, const char playerChar)
{
  if (grid == NULL){
    return mapchars_roomSpot;
  }

  char* charString = calloc(2, sizeof(char));
  mem_assert(charString, "out of memory\n");

  charString[0] = playerChar;
  charString[1] = '\0';

  char* pResult = hashtable_find(grid->playersStandingOn, charString);
  free(charString);

  if (pResult == NULL){
    return mapchars_roomSpot;
  }

  return *pResult;
}

/****************** setPlayerStandingOn *******************
 *
 * updates the grid's bookkeeping of what the player is standing on
 *
 */
static void
setPlayerStandingOn(grid_t* grid, const char playerChar, const char newChar)
{
  if (grid == NULL){
    return;
  }

  char* charString = calloc(2, sizeof(char));
  mem_assert(charString, "out of memory\n");
  charString[0] = playerChar;
  charString[1] = '\0';

  // We can't insert a char; we need to insert a pointer to one
  // We can't just use the memory location of new char as that's on the stack

  hashtable_t* ht = grid->playersStandingOn;
  char* pChar = hashtable_find(ht, charString);
  if (pChar == NULL){
  pChar = malloc(sizeof(char));
  mem_assert(pChar, "out of memory\n");
  hashtable_insert(ht, charString, pChar);
  } 

  *pChar = newChar;
  

  free(charString); // the key got copied
}

/****************** freeCharItemdelete ********************
 *
 * itemdelete for the standingOn hashtable
 * we cannot simply pass in 'free' because we need to deal with the null case
 *
 */
static void
freeCharItemdelete(void* pChar)
{
  if (pChar == NULL){
    return;
  }

  free(pChar);
}
