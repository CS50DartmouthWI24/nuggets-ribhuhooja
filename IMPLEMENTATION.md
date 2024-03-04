# CS50 Nuggets
## Implementation Spec
### Torpedos, Winter 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes Grid, Player, Spectator, Game.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

Our team has four members - Paul, Ribhu, Sam, Tayeb. Here is our plan for dividing the
labour between the team members.

### Paul
Paul will handle the client program completely. He will write the program,
using the curses module to provide a UI. He is also responsible for testing the
program.

### Ribhu
Ribhu will write the grid module. He is responsible for handling all the grid
game logic, including visibility, and testing the grid.

### Sam
Sam will write the server program. He is responsible for making sure the program
starts correctly and to handle/send messages to and from clients, and for making
sure that the right functions for the helper modules are called. He is also 
responsible for testing the sever module.

### Tayeb
Tayeb will write the game module. He is responsible for handling the game 
and making sure it integrates all the other modules correctly. He is also responsible
for testing the module.

### Notes
This division of labor is note strict, in the sense that we will dynamically
look at the amount of work needed for each person and try to distribute it evenly.
We especially anticipate a lot of overlap between the duties of the three people assigned to
server.

## Client

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	initialize message module
	print assigned port number
	decide whether spectator or player

---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## Grid

### Data structures

### Definition of function prototypes

### Detailed pseudo code

## Game


### Data structures


we use game data structure. It stores all the structure of the player inside it. Game sturctures includes the folowing:
```c
typedef struct game{
   player_t** players;         // Array of players. It stores all the player whether active or inactive.
   grid_t* masterGrid;         // The master grid(map) which covers the whole map.
   spectator_t* spectator;     // The address of the spectator.
   int numPlayer;              // Number of players joined the game so far.
   int goldRemain;             // The remaining gold in the game.
} game_t;
```
### Definition of function prototypes


A function to initialize the player. It takes only the mapfile as parameter.
```c
game_t* game_init(FILE* mapfile);
```

A function to add a new player to the game. It checks if player and game is null.
```c
void game_addPlayer(game_t* game, player_t* player);
```

A function to remove the player of the game. It checks if player and game is null.
```c
void game_removePlayer(game_t* game, player_t* player);
```

A function to add new spectator to the game. It removes spectator if there is already one in the game. It checks if address and game is null.
```c
void game_addSpectator(game_t* game, addr_t address);
```

A function to remove the spectator of the game. It checks if spectator and game is null.
```c
void game_removeSpectator(game_t* game, addr_t address);
```

A function to return the master grid of the game. It checks if game is null.
```c
grid_t* game_masterGrid(game_t* game);
```

A function to return the number of player in the game. It checks if game is null.
```c
int game_numPlayers(game_t* game);
```

A function to return the list of players in the game. It checks if game is null.
```c
player_t** game_getPlayers(game_t* game);
```

A function to return the amount of gold in the game. It checks if game is null.
```c
int game_getGold(game_t* game);
```

A function to return the spectator in the game. It checks if game is null.
```c
spectator_t* game_getSpectator(game_t* game);
```

A function to find the player and return the player with that address in the game. It checks if game is null.
```c
player_t* game_findPlayer(game_t* game, addr_t address);
```

A function to return true if the player moves in the game otherwise to false. It checks if game is null.
```c
bool game_move(game_t* game, addr_t address, int dx, int dy);
```

A function to return true if the player moves all the way down before it hits the wall on the map, otherwise false. It checks if game is null.
```c
bool game_longMove(game_t* game, addr_t address, int dx, int dy);
```

A function that called when all the gold in the game is claimed. It delete everthing in the game and send the result to all players and spectators. It checks if game is null.
```c
void game_over(game_t* game);
```


### Detailed pseudo code

#### `Initial Game`:

   If mapfile is not valid, return NULL
   Allocate memory for a new game structure
   Initialize the game with the following:
       - Read the map from mapfile to create the master grid
       - Allocate memory for players and spectators
       - Set numPiles based on the map
   Return the initialized game structure if successful, NULL if any failure occurs

#### `Add player`:

   If game or player is NULL, print an error message and do nothing
   If the game has less than 26 players and the player has not already joined:
       - Add the player to the game's array of players
   Else
       - Send an error message to the player
       - Deny the request to join the game

#### `Remove player`:

   If game or player is NULL, print an error message
   If the player has joined the game:
       - Send a quit message to the player
       - Mark the player as inactive (isActive = false)
   Else
       - Print an error message
   Note: Do not remove the player from the array; just mark as inactive

#### `Add spectator`:

   If game is NULL or address is invalid, do nothing
   If there is already a spectator in the game:
       - Replace the current spectator with the new one
       - Send a quit message to the current spectator
       - Initialize the new spectator
   Else
       - Accept the request and initialize the spectator

#### `Remove spectator`:

   If game is NULL or address is invalid, do nothing
   If a spectator is present:
       - Send a quit message to the spectator
       - Delete the spectator
       - Set the game's spectator to null for future spectators to join

#### `Get Master Grid`:

   If game is NULL, return NULL
   Return a pointer to the master grid of the game
   Note: Caller should not delete the grid; it's managed by game_delete

#### `Get Number of players`:

   If game is NULL, return 0
   Return the number of players that have joined the game, including inactive players

#### `Get List of Players`:

   If game is NULL, return NULL
   Return a pointer to the array of player pointers in the game
   Note: The array can be partially filled or NULL if no players have joined

#### `Get Gold`:

   If game is NULL, return -1
   Return the amount of gold remaining in the game

#### `Get Spectator`:

   If game is NULL, return NULL
   Return a pointer to the game's spectator

#### `Find Player`:

   If game is NULL or address is invalid, return NULL
   Search the game's player array for a player with the given address
   If found, return a pointer to the player
   Else, return NULL

#### `Move Player`:

   If game is NULL or address is invalid, do nothing
   Update the player's position based on dx and dy
   Update the player's visible grid and gold based on the move
   If stepping on a gold pile, update gold and send a message to the player
   Return true if this move causes the game to end, false otherwise

#### `Move Player Long`:

   If game is NULL or address is invalid, do nothing
   Move the player continuously until hitting a wall, updating grid and gold each step
   Return true if this move causes the game to end, false otherwise
   Notes: It calls some static functions to update the gold and visiblity
   for each step. It send the amount of the remaining game to all players and spectator.

#### `Game Over`:

   If game is NULL, do nothing
   Send a quit message to all players and the spectator
   Print the total gold claimed by each player
   Free all allocated memory for the game

## Player

### Data structures

we use player data structure which includes the folowing:

``` c
typedef struct player {
 int x;                 // X-coordinate of the player
 int y;                 // Y-coordinate of the player
 grid_t* visibleGrid;   // The grid that the player can see
 int gold;              // The gold collected by the player
 char* name;            // The name of the player
 bool isActive;         // Whether the player is active
 char letter;           // The character representation of the player on the map
 addr_t address;        // The address of the player client, for sending messages
} player_t;
```

### Definition of function prototypes

A function to make a new player. It takes address, x and y coordindates, the name of the player and also the letter assigned for the player.

```c
player_t* player_new (addr_t address, int x, int y, const char* name, char letter );
```

A function that deletes the player. It make sure to free memory for everything that is allocated within player.

```c
void player_delete(player_t* player);
```

A function that return the x coordinates of the player. It checks if the player is null.

```c
int player_getX(const player_t* player);
```

A function that return the y coordinates of the player. It checks if the player is null.

```c
int player_getY(const player_t* player);
```

A function that return the visible grid of the player. It checks if the player is null.

```c
grid_t* player_getVisibleGrid(const player_t* player);
```

A function that return the amount of gold in player's purse. It checks if the player is null.

```c
int player_getGold(const player_t* player);
```

A function that return the name of the player. It checks if the player is null.

```c
char* player_getName(const player_t* player);
```

A function that return the letter assigned for the player. It checks if the player is null.

```c
char player_getLetter(const player_t* player);
```

A function that return the address of the player. It checks if the player is null.

```c
addr_t player_getAddress(const player_t* player);
```

A function that checks if there is active or not. It checks if the player is null.

```c
bool player_isActive(player_t* player);
```

A function that sets the x coordinates of the player. It checks if the player is null.

```c
void player_setX(player_t* player, int x);
```

A function that sets the Y coordinates of the player. It checks if the player is null.

```c
void player_setY(player_t* player, int x);
```

A function that sets the amount of gold in player's purse. It checks if the player is null.

```c
void player_setY(player_t* player, int x);
```

A function that add to the amount of gold in player's purse. It checks if the player is null.

```c
void player_addGold(player_t* player, int gold);
```

A function that moves the player horizontally. It checks if the player is null.

```c
void player_moveX(player_t* player, int direction);
```

A function that moves the player vertically. It checks if the player is null.

```c
void player_moveY(player_t* player, int direction);
```

A function that moves the player diagonnaly. It checks if the player is null.

```c
void player_moveDiagonal(player_t* player, int Xdirection, int Ydirection);
```

A function that updates visible grid of the player. It checks if the player is null.

```c
void player_updateVisibleGrid(player_t* player, grid_t* masterGrid);
```

A function that sets the player to inactive when it leaves the game. It checks if the player is null.

```c
void player_setInactive(player_t* player);
```

A function that sends message to the player. It checks if the player is null.

```c
void player_sendMessage(player_t* player, char* message);
```


### Detailed pseudo code


#### `New player`:
Create a new player with given details:
- Check if the name is NULL. If it is, return NULL as we need a valid name.
- Allocate memory for a new player structure.
- Allocate memory for the player's name and copy the given name into this allocated space.
- Initialize the player's properties: x and y coordinates, letter, active status, address, gold amount (start with 0), and set the visible grid to NULL.
- Return the pointer to the newly created player.


#### `Delete player`:
- If the player pointer is NULL, do nothing.
- Free the memory allocated for the player's name.
- Delete the player's visible grid (if it exists).
- Free the memory allocated for the player structure.


#### `Get Y`:
- If the player is NULL, log an error and return -1.
- Otherwise, return the player's x-coordinate.


#### `Get Y`:
- If the player is NULL, log an error and return -1.
- Otherwise, return the player's y-coordinate.


#### `Get Visible grid`:
- If the player is NULL, log an error and return NULL.
- Otherwise, return the player's visible grid.


#### `Get Gold`:
- If the player is NULL, log an error and return 0.
- Otherwise, return the player's gold amount.


#### `Get Name`:
- If the player is NULL, log an error and return NULL.
- Otherwise, return the player's name.


#### `Get Letter`:
- If the player is NULL, log an error and return a null character.
- Otherwise, return the player's letter.


#### `Get Address`:
- If the player is NULL, log an error and return a no-address indicator.
- Otherwise, return the player's address.


#### `Is Active`:
- If the player is NULL, return false.
- Otherwise, return the player's active status.


#### `Set X`:
- If the player is NULL, log an error.
- Otherwise, update the player's Y-coordinate to the given value.


#### `Set Y`:
- If the player is NULL, log an error.
- Otherwise, update the player's Y-coordinate to the given value.


#### `Set Gold`:
- If the player is NULL, log an error.
- Otherwise, update the player's gold amount to the given value.


#### `Set Add Gold`:
- If the player is NULL, log an error.
- Otherwise, increase the player's gold by the given amount.


#### `Set Player Inactive`:
- If the player is NULL, log an error.
- Otherwise, set the player's active status to false.


#### `Move Horizontally`:
- If the player is NULL, log an error.
- Otherwise, adjust the player's x-coordinate by the given direction value.


#### `Move Vertically`:
- If the player is NULL, log an error.
- Otherwise, adjust the player's Y-coordinate by the given direction value.


#### `Move Diagonnaly`:
- If the player is NULL, log an error.
- Otherwise, adjust the player's x and y coordinates by the given X and Y direction values.


#### `Update Visible Grid`:
- If the player is NULL, log an error.
- Generate a new visible grid for the player based on their current location and the master grid, then update the player's visible grid to this new grid.


#### `Send Message`:
- If the player is NULL, log an error.
- Otherwise, use the player's address to send them the given message.


## Spectator

### Data Structure

We use only spectator data structure here which holds the address of the spectator


```c
typedef struct spectator {
 addr_t address;
} spectator_t;
```

### Definition of function prototypes

A function to make a new spectator. It takes address of the spector as a parameter.

```c
spectator_t* spectator_new(addr_t address);
```

A function that deletes the spectator. It make sure to free memory for grid that is allocated within spectator.

```c
void spectator_delete(spectator_t* spectator);
```

A function that return the address of the spectaor. It checks if the spectator is null.

```c
addr_t spectator_getAddress(spectator_t* spectator);
```

A function that sends message to the spectator. It checks if the spectator is null.

```c
void spectator_sendMessage(spectator_t* spectator, char* message);
```

### Detailed pseudo code

#### `New Spectator`:
Create a new spectator with given details:
- Check if the address. Return null if it the address is null.
- Allocate memory for a new spectator structure.
- Return the pointer to the newly created spectator.

#### `Delete spectator`:
- If the spectator pointer is NULL, do nothing.
- Free the memory allocated for the plaspectatoryer structure.

#### `Send Message`:
- If the spectator is NULL, log an error.
- Otherwise, use the spectator's address to send them the given message.

#### `Get Address`:
- If the spectator is NULL, log an error and return a no-address indicator.
- Otherwise, return the spectator's address.

---

## Testing plan

### unit testing
#### Grid
Grid will be unit tested by a simple C driver that is just going to perform
some grid actions and print out the resulting grid to stdout. Since the grid
is a string, this does not require any displaying program to work.

#### Game

Because Game is essential to the entire system, we test game whenever we test client and server. These conceptual links and dependencies allow us to debug Game while running server.

#### Player

Because Player is essential to the entire system, we test game whenever we test client and server. These conceptual links and dependencies allow us to debug Player while running server.

### integration testing

We will use the mini client and mini server modules given in the starter kit to effectively test our programs as we are writing them. For testing client, we will set up a miniserver and try to pass keystrokes with the correctly formatted messages. Additionaly we will manaully call methods in the main (acting as a driver) to verify that when correcly formatted messages are given to the client, it dispalys correctly. For instance, we will call handleOK with an OK message, then handleGRID with a GRID message, then handleDISPLAY with a sample map DISPLAY message and ensure that the manually created messages produce the correct output on the screen.

For server, we will use miniclient to send messages to and engage in a similar process to testing client. Additionally,we hope server will come together after client, so in theory we can use a more finished version of client to correctly test server.

### system testing


We will test our program on a series of maps, given in the `maps` directory in the repository. We will test with several clients joining at once on different maps. We will test the functionality of bumping and switching spots with players, in rooms, hallways, and make sure that visibility and lines of sight are always correct. Of course we are also careful to check that gold status bars on each of our clients are consitently updated (correctly) and the final scoreboard also reflects the scores that we had expected to see. 

---

## Limitations

### Strict update
We re-evaluate the grid for each player (on the server) when ANY player moves.
This is somewhat inefficient, because the changes might not have been visible to some players.
But to keep it simple, we have chosen not to handle those complex visibility calculations.

(We CAN avoid sending the same string over the network, though, if we compare against the previous
grid string and check if it's the same.)

e.g. if player 1 is in room 1 and players 2 & 3 are in room 2, then if player
2 moves then player 1's display doesn't change. But player 1 nevertheless still 
receives a display message from the grid.

### Simple grid refresh
When the client displays a grid, it displays the whole grid as received
after wiping the previous grid. This is inefficient, because the vast majority of
points on the screen did not change. To keep it simple, we do not do a smart grid
refresh which would only change the changed poritions of the grid.

### Redundant visibility checks
The way we plan to check visibility involves checking each point on the grid. 
This is somewhat redundant, as many points that lie on the same 'ray" through the 
player will have the same visibility value. However, unless this implementation ends
up being too slow, we don't plan to optimize out this redundancy.
