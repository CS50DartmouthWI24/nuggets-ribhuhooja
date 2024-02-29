/*
 * gridtest - a unit test for the grid module
 * 
 * It is a simple C-driver, and doesn't do much error checking besides
 * what is required to test grid
 *
 * Ribhu Hooja, February 2024
 */

#include <stdio.h>
#include <stdlib.h>
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

  printf("Test: adding player, getting player to move\n\n");
  grid_addPlayer(grid, 5, 2, '@');
  grid_toMap(grid, stdout);
  printf("\n\n");
  grid_movePlayer(grid, 5, 2, 0, 1); // should fail
  grid_toMap(grid, stdout);
  printf("\n\n");
  grid_movePlayer(grid, 5, 2, -1, 0);
  grid_toMap(grid, stdout);
  printf("\n\n");

  printf("Test: The hallway problem, also more movement checks\n\n");
  grid_movePlayer(grid, 4, 2, 0, 1); 
  grid_movePlayer(grid, 4, 3, 0, 1); 
  grid_toMap(grid, stdout);
  printf("\n");
  grid_movePlayer(grid, 4, 4, 1, 0); // should fail
  grid_toMap(grid, stdout);
  printf("\n");
  grid_movePlayer(grid, 4, 4, 0, 1); 
  grid_movePlayer(grid, 4, 5, 0, 1); 
  grid_movePlayer(grid, 4, 6, 0, 1); 
  grid_movePlayer(grid, 4, 7, 0, 1); 
  grid_toMap(grid, stdout);
  printf("\n");

  printf("Test: removing players\n");
  grid_removePlayer(grid, '@', 4, 8);
  grid_toMap(grid, stdout);
  printf("\n");

  printf("Test: Gold nuggets. Using preset seed to have consistent test behavior\n\n");

  srand(42);
  grid_nuggetsPopulate(grid, 5, 10, 30);
  grid_toMap(grid, stdout);
  printf("\nDebug: See the gold amounts\n");
  grid_debugGoldNumbersToMap(grid, stdout);
  printf("\n");

  printf("Test: Have a player collect the gold\n\n");
  printf("Gold at (6, 1): %d\n", grid_goldAt(grid, 6, 1));
  grid_addPlayer(grid, 6, 2, '$');
  grid_toMap(grid, stdout);
  printf("\n");
  int goldCollected = grid_movePlayer(grid, 6, 2, 0, -1);
  grid_toMap(grid, stdout);
  printf("\n");
  printf("Gold collected: %d\n", goldCollected);
  grid_movePlayer(grid, 6, 1, 0, 1);
  grid_toMap(grid, stdout);
  printf("\n");
  grid_debugGoldNumbersToMap(grid, stdout);
  printf("\n");



  // TODO: Check
  // nuggetsPopulate, goldAt, collectGold
  // visibility - probably needs to be checked separately 



  grid_delete(grid);

  return 0;
}

