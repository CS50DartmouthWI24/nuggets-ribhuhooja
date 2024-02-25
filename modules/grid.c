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
#include "grid.h"

/****************** types ********************************/
typedef struct grid_t {
  char* string;         // the string representation of the grid
  int numrows;          // number of rows
  int numcols;          // number of columns
  counters_t* nuggets;  // number of nuggets at a location, keyed by string index
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
static bool isVisible(grid_t* grid, int px, int py, int x, int y);

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

  new->nuggets = ctrs;
  
  return new;
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
