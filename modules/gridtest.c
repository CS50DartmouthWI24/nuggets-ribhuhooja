/*
 * gridtest - a unit test for the grid module
 * 
 * It is a simple C-driver, and doesn't do much error checking besides
 * what is required to test grid
 *
 * Ribhu Hooja, February 2024
 */

#include <stdio.h>
#include "grid.h"

/****************** main *********************************/
int
main()
{
  FILE* fp = fopen("../maps/fewspots.txt", "r");
  if (fp == NULL){
    printf("null fp\n");
    return 1;
  }

  grid_t* grid = grid_fromMap(fp);
  fclose(fp);

  if (grid == NULL){
    printf("Null grid\n");
    return 1;
  }

  grid_toMap(grid, stdout);

  grid_delete(grid);

  return 0;
}

