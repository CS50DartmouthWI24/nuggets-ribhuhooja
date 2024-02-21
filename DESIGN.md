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

1. `parseArgs`, which parses the command line arguments
2. `messageHandler`, which handles the messages received from the server
3. `inputHandler`, which handles player input
4. `displayer`, which displays the grid and status line to the user
 
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
    
The client uses a data structure `status` to keep track of the status line.

It doesn't need any other data structure to keep track of the grid or player position
because the map to display is sent by te server.
    
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

The server is composed of the following modules (other than main)

1. `parseArgs`, parses the command line arguments to the server
2. `gameInitializer`, which sets up the data structures
3. `messageHandler`, which handles messages
4. `gameUpdater`, which updates the game based on player actions


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


#### game initializer

  load and validate map from the given map file into a grid object
  places nuggets in the grid
  
### Major data structures

#### Grid
A data structure to represent a grid. Stores:

- the grid string
- a `counterset` (from libcs50) of nuggets - indexed by the index of the position in the grid string

#### Player
A data structure to store the data of each player. Stores:

- x and y coordinates
- grid visible to the player
- amount of gold
- name
- assigned letter
- address (to send messages to)

### Functional Decomposition
`player_join` - joins a player into a game
`player_leave - leaves a player from a game
`player_collectGold` - adds the current gold on player (x,y) coordinate
`player_move` - moves a player into a spot on the grid
#### Game
A data structure to hold global game state. Stores:

- the 'base' grid which is the actual game map (as opposed to the
limited-visibility grids seen by each player
- the total amount of nuggets left
- an array of active players
- an array of removed players

---

## Grid
This module implements the grid datastructure, which provides an abstraction for the 
game grid

### Functional decomposition

  - `grid_fromMap` - creates a new grid from a map file
  - `grid_charAt` - returns the character at a given point
  - `grid_goldAt` - returns the amount of gold at a given point
  - `grid_generateVisibleGrid` - given the base grid and the player, it generates each player's visible grid
  - `grid_movePlayer` - moves the players within the grid
  - `grid_collectGold` - changes the grid to represent gold being collected by a player
  - `grid_display` - displays/prints the grid string
  - `grid_toMap` - saves the grid to a file for debug purposes

Static helper functions

  - `indexOf` - returns the index of an (x,y) point in the string
  - `isVisible` - tells whether a point is visible from another

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

#### move player

  check if the player can move
  move the player coordinates
  swap players if needed
  if there is gold at the newly moved to location, call collectGold
  update the grid string to reflect the changes
  message all the players to inform them of the change

#### collect gold

  get the amount of gold at the given position
  add that gold to the player's purse
  update the game state to a new amount of gold
  update the counterset; set the amount of gold at that index to 0
  send the GOLD message to all the players to inform them of the change

We do not update the grid string, because collect gold is only ever called when a player steps into that
spot, and so the asterisk at that spot is replaced by a player character by move player

#### display

  print the grid string

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

A data structure to store the data of each player. Stores:

- x and y coordinates
- grid visible to the player
- amount of gold
- name
- assigned letter
- address (for the server to send messages to)

### Functional decomposition

The player is composed of the following modules (other than main)

`player_join` - joins a player into a game
`player_leave` - leaves a player from a game
`player_move` - moves a player into a spot on grid
`handleMessage` - handles a message from the server and updates the player data structure variables accordingly (will call the above methods)

### Pseudo code for logic/algorithmic flow

> For any non-trivial function, add a level-4 #### header and provide tab-indented pseudocode.
> This pseudocode should be independent of the programming language.

### Major data structures

We use a `grid` data structure to store the visibility grid of the player. All other aspects of the `player` struct will be defined as integers thus do not need a data struct

## Game

The game data structure will keep track of the following data which defines a game. 

`grid_t*` stores the underlying grid with full visibility (as opposed to the limited-visibility grids seen by each player)

`player_t** activePlayers` an array of the active players who are playing the game
`player_t** inactivePlayers` an array of the active players who have finished playing the game

`int maxNameChars` stores the maximum number of charachters a player can name themselves
`int maxPlayers` stores the maximum number of players a game can have
`int goldTotal` stores the total amount of gold ot be distributed
`int minNumGoldPiles` stores the minimum number of goldPiles
`int maxNumGoldPiles` stores the maximum number of goldPiles

> Repeat this section for each module that is included in either the client or server.

### Functional decomposition

    `addClient` - adds a client with a port number to the array of clients
    `removeClient` - removes a client with a port number from the array of clients
    `endGame` - handles game over

### Pseudo code for logic/algorithmic flow

#### endGame

  change display from map to a blank screen
  display leaderboard of gold
  while (players in game)
    removeClient
  close port


### Major data structures
We make ample use of `grid_t*` and `player_t*` data structures to store the `grid_t*` that defines the underlying map and a `player_t*` that defines each player who is added to the game



