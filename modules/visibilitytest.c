/*
 * a file to test the behavior of the visibility grid feature
 * 
 * Doesn't do much error checking beyond what's needed to test grid
 * because it is just a simple C-driver for grid functions
 *
 * Ribhu Hooja, February 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "mapchars.h"

/****************** main *********************************/
int
main()
{
  FILE* fp = fopen("../maps/main.txt", "r");
  grid_t* grid = grid_fromMap(fp);
  fclose(fp);

  FILE* log = fopen("logtest.txt", "w");

  // checking if everything works
  grid_toMap(grid, stdout);

  // emulating a player object
  int px = 9;
  int py = 18;
  char letter = 'A';
  grid_t* visibleGrid = NULL;

  srand(42);
  grid_nuggetsPopulate(grid, 10, 30, 250);

  printf("Adding a player - this is the base grid\n\n");
  grid_addPlayer(grid, px, py, letter);
  grid_toMap(grid, stdout);

  printf("Now calculating the grid visible to the player\n\n");
  visibleGrid = grid_generateVisibleGrid(grid, visibleGrid, px, py);
  grid_toMap(visibleGrid, stdout);

  printf("beginning logging..\n");
  for (int i = 0; i < 5; ++i){
    grid_movePlayer(grid, px, py, 0, -1);
    --py;
    grid_generateVisibleGrid(grid, visibleGrid, px, py);
    grid_toMap(visibleGrid, log);
  }

  for (int i = 0; i < 27; ++i){
    grid_movePlayer(grid, px, py, 1, 0);
    ++px;
    grid_generateVisibleGrid(grid, visibleGrid, px, py);
    grid_toMap(visibleGrid, log);
  }
  
  for (int i = 0; i < 3; ++i){
    grid_movePlayer(grid, px, py, 1, 1);
    ++px;
    ++py;
    grid_generateVisibleGrid(grid, visibleGrid, px, py);
    grid_toMap(visibleGrid, log);
  }
  
  for (int i = 0; i < 10; ++i){
    int res = grid_movePlayer(grid, px, py, 1, 0);
    if (res != -1){
      ++px;
    }
    grid_generateVisibleGrid(grid, visibleGrid, px, py);
    grid_toMap(visibleGrid, log);
  }
  
  for (int i = 0; i < 10; ++i){
    int res = grid_movePlayer(grid, px, py, 0, -1);
    if (res != -1){
      --py;
    }
    grid_generateVisibleGrid(grid, visibleGrid, px, py);
    grid_toMap(visibleGrid, log);
  }


  fclose(log);

  grid_delete(grid);
  grid_delete(visibleGrid);
}
