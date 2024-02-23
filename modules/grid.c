/*
 * grid.c - a file implementing the grid module for 
 * the cs50 nuggets game
 *
 * usage and description is given in grid.h
 *
 * Ribhu Hooja, February 2024
 */

#include <stdio.h>
#include <stdbool.h>
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

/****************** local function prototypes ************/
static inline int indexOf(const int x, const int y, const int numcols);
static bool isVisible(grid_t* grid, int px, int py, int x, int y);

/****************** global function prototypes ***********/
/* see grid.h for description and usage */





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
