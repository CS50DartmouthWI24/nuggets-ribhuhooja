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
  printf("Test: Loading map\n\n");
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

  printf("Test: Displaying map, numrows, numcols\n\n");
  grid_toMap(grid, stdout);
  printf("\nnumber of rows: %d\n number of columns: %d \n", grid_numrows(grid), grid_numcols(grid));


  printf("Test: CharAt\n\n");
  printf("char at (2,0) is %c\n", grid_charAt(grid, 2, 0));
  printf("char at (3,1) is %c\n", grid_charAt(grid, 3, 1));
  printf("char at (4,3) is %c\n", grid_charAt(grid, 4, 3));



  grid_delete(grid);

  return 0;
}

