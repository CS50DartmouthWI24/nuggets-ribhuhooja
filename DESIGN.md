# CS50 Nuggets
## Design Spec
### Torpedos, Winter 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes player, grid, and gridpoint modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Client

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.
    
### Inputs and outputs

#### Inputs
Besides the command line arguments, the client will listen for individual keystrokes (without the need of a newline to break it up). These inputs will then be sent to the server

#### Outputs
The output of the program will be a visual, realtime display of the game grid. 
The program will also display a status line. See the requirements spec for the format of the status line.

### Functional decomposition into modules

Our client comprises the following modules or functions:

`static bool handleInput(void* arg)` which handles the input from the clients' keyboard
`static bool handleMessage(void* arg, const addr_t from, const char* message)` which handles the message sent to the client from the server
`static int parseArgs(const int argc, char* argv[], clientData_t* cData)` which parses and verifies the commmand line arguments
`static void initializeTerminal(void)` which initalizes an ncurses terminal
`static void handleGRID(const char* message, void* arg)` which handles the GRID message from server
`static void handleGOLD(const char* message, void* arg)` which hadnles the GOLD message from server
`static void handleQUIT(const char* message, void *arg)` which handles the QUIT message from server
`static void handleERROR(const char* message)` which handles the ERROR message from server
`static void handleDISPLAY(const char* message, void* arg)` which handles the DISPLAY message from server
`static void handleOK(const char* message, void* arg)` which handles the OK message from server
 
### Pseudo code for logic/algorithmic flow

The client will run as follows:

  parse the command line arguments
  set up the client-server connection
  send the PLAY or SPECTATE message to the server
  message loop with the input handler and message handler

#### input handler
  take user keystrokes from stdin and send them to the server as a KEY message

#### messageHandler
  receive messages from the server
  if the display needs to be updated, update the displaye
  if the server sends a quit message, quit

### Major data structures
    
The client uses a data structure `cData` to keep track of the data needed for displaying messages at any point in the program. It serves as a useful way to package data needed
    
---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

#### Inputs
The server has no inputs from stdin, but handles messages from clients.

#### Outputs
At the beginning, the server prints the port number to stdout.
The server outputs log messages to stderr. It 'outputs' messages to clients
to tell them about the game state.

### Functional decomposition into modules

The server is composed of the following functions (other than main)

`static void parseArgs(const int argc, char* argv[], FILE** map, int* seed)` which parses and verifies the arguments
`static bool handleMessage(void* arg, const addr_t from, const char* buf)` which handles the messages coming from the client
`static void handlePlay(void* arg, const addr_t from, const char* content)` which handles the PLAY message sent client
`static bool handleKey(void* arg, const addr_t from, const char* content)` which handles the KEY message sent from the client
`static void handleSpectate(void* arg, const addr_t from, const char* content)` which handles the SPECTATE message sent from the client
`static void keyQ(const addr_t from)` which handles the KEY Q message sent from the client
`static void errorMessage(const addr_t from, const char* content)` which is called when someone hits an improper key and tells the user so
`static bool checkWhitespace(const char* name)` which takes a name and returns whether it is empty or not
`static char* fixName(const char* entry)` which normalizes the string with the requirements of the specs (truncating, and adding underscores if neccesary)


### Pseudo code for logic/algorithmic flow

The server will run as follows:

	execute from a command line per the requirement spec
	parse the command line, validate parameters
	call initializeGame() to set up data structures
	initialize the 'message' module
	print the port number on which we wait
	call message_loop(), to await clients
	call gameOver() to inform all clients the game has ended
	clean up


  
### Major data structures

#### Grid
A data structure to represent a grid. Stores:

- the grid string
- a `counterset` (from libcs50) of nuggets - indexed by the index of the position in the grid string
- a `hashtable` (from libcs50) to store what character each player is standing on - keyed by a string version of the player's letter

#### Player
A data structure to store the data of each player. Stores:

- x and y coordinates
- grid visible to the player
- amount of gold
- name
- assigned letter
- address (to send messages to)


#### game

- Array of players
- Master Grid to old the whole map
- The spectator
- Number of players joined the game so far
- The remaining gold in the game


-------

## Grid
This module implements the grid datastructure, which provides an abstraction for the 
game grid

### Functional decomposition

  1. `grid_fromMap` which creates a new grid from a map file
  2. `void grid_delete(grid_t* grid)` which deletes the grid and frees memory
  3. `grid_charAt` which returns the character at a given point
  4. `char grid_baseCharAt(grid_t* grid, const int x, const int y)` which returns the base character of the underlying map at the passed coordinate (what is in the map file)
  5. `int grid_numrows(grid_t* grid)` which returns the number of rows in grid
  6. `int grid_numcols(grid_t* grid)` which returns the number of columns in grid
  7. `grid_goldAt` - returns the amount of gold at a given point
  8. `bool grid_nuggetsPopulate(grid_t* grid, const int minNumPiles, const int maxNumPiles, const int goldTotal)` which adds nuggets within the specified parameters to the map
  9. `bool grid_findRandomSpawnPosition(grid_t* grid, int* pX, int* pY)` which finds a random empty spot on the map and populates the passed coordiantes by reference to the coordaintes of that spot
  10. `grid_generateVisibleGrid` - given the base grid and the player, it generates each player's visible grid
  11. `grid_addPlayer` which adds a player to a grid
  12. `grid_movePlayer` which moves the players within the grid
  13. `void grid_swapPlayers(grid_t* grid, const int x1, const int y1, const int x2, const int y2)` which swaps two players on the grid who are on (x1,y1) and (x2,y2) points on the map
  14. `bool grid_removePlayer(grid_t* grid, const char playerChar, const int px, const int py)` which removes a player on the map 
  15. `grid_collectGold` which changes the grid to represent gold being collected by a player
  16. `grid_getDisplay` which returns the grid string
  17. `grid_toMap` which saves the grid to a file for debug purposes

### Pseudo code for logic/algorithmic flow

#### fromMap

  open the map file
  read the map file character-by-character into the grid

#### charAt

  convert the coordinate to an index
  index into the string to get the character

#### goldAt

  convert the coordinate to an index
  return the result from `counters_get` on the nuggets counterset  using the index as a key

#### generate visible grid

  take a player and the base grid
  use the isVisible function with all points to figure out which points to show players and nuggets on
  use the player's already visible grid to show which points are 'remembered' - show the rooms, but not nuggets or players
  replace the player's letter by @

We can probably optimize the visibility algorithm to not check against every single
other point, but for now we have decided to take the simple approach. If that approach ends up being too slow,
we will optimize it.

#### add player

  check if the provided location is add-able
  put the player character at that location on the grid

Does not handle checking if the add is valid accoring to any rules other than location.
Does NOT allow adding on a location that has gold on it.

#### move player

  check if the player can move
  move the player coordinates
  swap players if needed
  if there is gold at the newly moved to location, call collectGold
  update the grid string to reflect the changes

Does NOT handle messaging
#### collect gold

  get the amount of gold at the given position
  add that gold to the player's purse
  update the counterset; set the amount of gold at that index to 0

Does not handle messaging or updating the game state.
We do not update the grid string, because collect gold is only ever called when a player steps into that
spot, and so the asterisk at that spot is replaced by a player character by move player

#### getDisplay

  return the grid string

#### to map

  put the string to the specified file

### Major data structures

This module defines the grid data structure, which is specified below.
It exposes the structure as an opaque type.

#### Grid
A data structure to represent a grid. Stores:

- the grid string
- a `counterset` (from libcs50) of nuggets - indexed by the index of the position in the grid string

## Player


### Major data structures

A data structure to store the data of each player. Stores:

- x and y coordinates
- grid visible to the player
- amount of gold
- name
- assigned letter
- address (for the server to send messages to)
- boolean to check if the player is active

### Functional decomposition

The player is composed of the following modules (other than main)

### Functional Decomposition
1. `player_t* player_new(addr_t address, int x, int y, const char* name, char letter)` which creates a new player with address, at x and y with name and letter ID
2. `void player_delete(player_t* player)` which deletes a player
3. `int player_getX(const player_t* player)` which gets the X value of a player
4. `int player_getY(const player_t* player)` which gets the Y value of the player
5. `grid_t* player_getVisibleGrid(const player_t* player)` which gets the visible grid for the player at their x and y
6. `int player_getGold(const player_t* player)` which gets the gold of the player
7. `char* player_getName(const player_t* player)` which gets the name of the player
8. `char player_getLetter(const player_t* player)` which gets the letterID of the player
9. `addr_t player_getAddress(const player_t* player)` which gets the addr_t adress of the player
10. `bool player_isActive(player_t* player)` which returns true or false if the player is player is playing (active)
11. `void player_setX(player_t* player, int x)` which sets the x of the player
12. `void player_setY(player_t* player, int y)` which sets the y of the player
13. `void player_setGold(player_t* player, int gold)` which sets the gold of the player
14. `void player_addGold(player_t* player, int gold)` which adds to the gold of the player
15. `void player_moveX(player_t* player, int direction)` which moves the x of the player in int direction
16. `void player_moveY(player_t* player, int direction)` which moves the y of the player in int direction
17. `void player_moveDiagonal(player_t* player, int Xdirection, int Ydirection)` which moves the player in the diagonal direction 
18. `void player_updateVisibleGrid(player_t* player, grid_t* masterGrid)` which updates the visible grid
19. `void player_setInactive(player_t* player)` whicih sets the layer to be inactive 
20. `void player_sendMessage(player_t* player, char* message)` which sends the message to a player
21. `bool player_isActive(player_t* player)` which returns if player is active or not

### Pseudo code for logic/algorithmic flow

The player module will just include simple functions.


--------


## Game

### Major data structures

The game data structure will keep track of the following data which defines a game. 

- `grid_t*` stores the underlying grid with full visibility (as opposed to the limited-visibility grids seen by each player)

- `player_t** activePlayers` an array of the active players who are playing the game
- `player_t** inactivePlayers` an array of the active players who have finished playing the game

- `int maxNameChars` stores the maximum number of charachters a player can name themselves
- `int maxPlayers` stores the maximum number of players a game can have
- `int goldTotal` stores the total amount of gold ot be distributed
- `int minNumGoldPiles` stores the minimum number of goldPiles
- `int maxNumGoldPiles` stores the maximum number of goldPiles


### Functional decomposition

  1. `game_t* game_init(FILE* mapfile)` to initialize the game 
  2. `void game_addPlayer(game_t* game, player_t* player)` to add new player in the game
  3. `void game_removePlayer(game_t* game, player_t* player)` to remove the player
  4. `void game_addSpectator(game_t* game, addr_t address)` to add spectator
  5. `void game_removeSpectator(game_t* game, addr_t address)` to remove spectator
  6. `grid_t* game_masterGrid(game_t* game)` to get the master map of the game
  7. `int game_numPlayers(game_t* game)` to get number of the players joined so far
  8. `player_t** game_getPlayers(game_t* game)` to get list of players in the game
  9. `int game_getGold(game_t* game)` to get the amount of gold in the game
  10. `spectator_t* game_getSpectator(game_t* game)` to get the spectator in the game
  11. `player_t* game_findPlayer(game_t* game, addr_t address)` to find the player in the game 
  12. `bool game_move(game_t* game, addr_t address, int dx, int dy)` to check if a player moved in the game
  13. `bool game_longMove(game_t* game, addr_t address, int dx, int dy)` to tcheck if the player moves all the way down before it hits the wall



### Pseudo code for logic/algorithmic flow

The game module will just consist of helper functions.

`static void sendGoldMessage(game_t* game, player_t* player, const int goldCollected, const int purse, const int goldRemaining)` to send the gold to a payer 
`static void sendAllGoldMessages(game_t* game, player_t* goldJustCollectedPlayer, int goldJustCollected)` to send the gold messsage to allplayer
`static void displayAllPlayers(game_t* game)` to display the all players 
`static void updateAllVisibleGrids(game_t* game)` to update the visiblity of all players
`static char* get_result(game_t* game)` to get the result when the game is over
`static player_t* findPlayerByCoords(game_t* game, const int x, const int y)` to find a player by coordinates





