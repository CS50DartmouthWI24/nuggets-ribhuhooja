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
starts correctly and to handle messages from clients, and for making
sure that the right functions for the helper modules are called. He is also 
responsible for testing the server module.

### Tayeb
Tayeb will write the game module. He is responsible for handling the game,
making sure it integrates all the other modules correctly, and making sure it
sends the right messages when the right actions happen. He is also responsible
for testing the module.

### Notes
This division of labor is not strict, in the sense that we will dynamically
look at the amount of work needed for each person and try to distribute it evenly.
We especially anticipate a lot of overlap between the duties of the three people assigned to
server.

### Retrospective
In retrospect, there was a lot of overlap over the people who were working
on server-side funtionality. A lot of changes were needed especially in game,
because it was the connecting link between server and grid. However, the work 
distribution ended up being fair and we were able to distribute different
features evenly, if a little differently from what was originally imagined.

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
The data structure provided by the grid module is the `grid_t` data structure.
The structure is exposed as an opaque type, but internally, it is defined as:

```c
typedef struct grid {
  char* string;         // the string representation of the grid
  int numrows;          // number of rows
  int numcols;          // number of columns
  counters_t* nuggets;  // number of nuggets at a location, keyed by string index
  hashtable_t* playersStandingOn;   // what character each player is standing on
} grid_t;
```

Inside the grid structure, the actual grid is represented as a single string,
where each row is delimited by a newline. This makes it very easy to display
the grid or print it to a file. It also stores the numbers of rows and columns
in the grid.

A note about the grid implementation:
- x increases from left to right. Goind left is negative, going right is positive.
- y increases from up to down. Going up is negative, goin down is positive.

It stores a counterset that contains information about gold nuggets. The 
counterset is keyed by the index of the location of the gold in the grid
string, and the count is the amount of gold at that location.

It also stores a hashtable which contains information about what character
each player is 'standing' on. This effectively lets us reconstruct the 
'base map' from the current grid string, as the only characters that differ
are player characters and gold nuggets. Gold nuggets always 'stand' on room
spots, so we only need to store what each player is standing on. The hashtable
is keyed by the letter of the player (technically, because keys need to be
string, the key is the string with the letter being the first element and 
the null character being the second) and the value is a pointer to a copy of
the character the player is standing on (see pseudocode below or comments in
`grid.c` for an explanation of the pointer fiddling with the hashtable)


### Definition of function prototypes
Here are the function prototypes for functions exported by the grid module.
All the functions are described by a comment in `grid.h` and so those descriptions
are not repeated here.

```c
grid_t* grid_fromMap(FILE* mapFile);
void grid_delete(grid_t* grid);
int grid_numrows(grid_t* grid);
int grid_numcols(grid_t* grid);
char grid_charAt(grid_t* grid, const int x, const int y);
char grid_baseCharAt(grid_t* grid, const int x, const int y);
int grid_goldAt(grid_t* grid, const int x, const int y);
bool grid_nuggetsPopulate(grid_t* grid, const int minNumPiles, const int maxNumPiles, const int goldTotal);
grid_t* grid_generateVisibleGrid(grid_t* grid, grid_t* currentlyVisibleGrid, const int px, const int py);
bool grid_findRandomSpawnPosition(grid_t* grid, int* pX, int* pY);
bool grid_addPlayer(grid_t* grid, const int x, const int y, const char playerChar);
int grid_movePlayer(grid_t* grid, const int px, const int py, const int x_move,
void grid_swapPlayers(grid_t* grid, const int x1, const int y1, const int x2, const int y2);
bool grid_removePlayer(grid_t* grid, const char playerChar, const int px, const int py);
int grid_collectGold(grid_t* grid, const int px, const int py);
char* grid_getDisplay(grid_t* grid);
void grid_toMap(grid_t* grid, FILE* fp);
```

Static functions - again, the descriptions are written over their definitions
in `grid.c`

```c
static inline int indexOf(const int x, const int y, const int numcols);
static void getCoordsFromIndex(const int index, const int numcols, int* pX, int* pY);
static inline bool isValidCoordinate(const int x, const int y, const int numrows, const int numcols);
static char getPlayerStandingOn(grid_t* grid, const char playerChar);
static void setPlayerStandingOn(grid_t* grid, const char playerChar, const char newChar);
static bool isVisible(grid_t* grid, const int px, const int py, const int x, const int y);
static bool isBlockedHorizontally(grid_t* grid, const int px, const int py, const int x,  const int y);
static bool isBlockedVertically(grid_t* grid, const int px, const int py, const int x,  const int y);
static bool isBlocking(grid_t* grid, const int x, const int y);
static void freeCharItemdelete(void* pChar);
```

### Detailed pseudo code

#### `grid_fromMap`
  
  if the file pointer is NULL
    return NULL
  malloc a string of size initGridStringSize on the heap
  store its size in stringBufSize
  while the current character in file is not a newline
    if the string is too small to place another character into the string (using stringBufSize)
      realloc it to double its size
      store the new size in stringBufSize
    place the character into the string
  store the number of columns of the grid, equal to the number of characters read
  initialize numrows at 0
  while the current character is not EOF
    if the string is too small to place another character into the string (using stringBufSize)
      realloc it to double its size
      store the new size in stringBufSize
    read the character into the string
    if the currently read character is a newline
      increment the number of rows
  check that the map read is consistent, i.e. number of characters read matches the number of rows and columns read
  if not
    free string
    print some error messages
    return NULL
  now that the map has been read, realloc the string to chop off the empty unused memory at the end
  malloc a new grid, using `mem_assert` to check that it is not NULL
  store the string, numrows and numcols inside the grid
  create a new hashtable and counterset, again asserting that they are not NULL
  store the pointers inside the grid
  return the newly formed grid

#### `grid_delete`

  if grid is NULL
    return, doing nothing

  free the grid string if it is not NULL
  delete the hashtable and counterset if they are not null
  free the grid itself

#### `grid_numrows`

  if the grid is NULL return 0
  return the number of rows in the grid

#### `grid_numcols`

  if the grid is NULL return 0
  return the number of rows in the grid

#### `grid_charAt`
In this function we check for coordinates being valid because we don't want any memory
errors when indexing into the string
  
  if grid is NULL return the null character
  check that the given coordinates are valid
  find the index of the given x and y coordinates
  return the character at that index in the grid string
   

#### `grid_baseCharAt`

  if grid is NULL return the null character
  get the charAt the given location
  if the returned character is any of the mapchars other then gold
    return it
  if the returned char is the gold nugget
    return the room character
  else (the character is a player character)
    return the character that the player is standing on using the helper function

#### `grid_goldAt`

  if grid is NULL or the counterset inside grid is NULL
    return 0
  otherwise, return the count of the index of the given coordinates inside the counterset

#### `grid_nuggetsPopulate`
The algorithm for populating with gold is
- first choose a random number between minNumPiles and maxNumPiles
- find that many spots on the map and save their indices
- now for each single piece of gold, randomly add it to one of the piles
  
  if grid is NULL
    return false
  check the given parameters are valid
  if not
    return false
  loop over the grid string, counting the number of room spots
  if there aren't enough room spots
    return false
  choose a random number between the min and max as the number of piles
  initialize an array to store the chosen gold pile spots
  keep looping until enough spots have been chosen
    choose a random index in the array
    if it is a room spot
      add its location to the array
      replace its character with a gold character
      set the value of that index in the nuggets counterset to 0
  now loop goldTotal times
    randomly choose one of the chosen pile spots
    add a single gold to the counterset at that spot
  return true
    

#### `grid_generateVisibleGrid`
This both updates the grid passed into it and returns it. This is useful 
for when a player has not been initialized with a visible grid yet, because it
NULL is passed in as the reference visible grid, the function generates a new grid
which is then (inside the player module) assigned to the player.

The grid that this function creates does not have the full functionality of
a grid and should only be used for display purposes. This is because it lacks the 
nuggets counterset and the standingOn hashtable.

  if grid is NULL or the given coordinates are invalid
    return NULL
  if the passed in currentlyVisibleGrid is NULL
    create a new grid, passing in the numrows and numcols but setting its counterset and hashtable to NULL
    malloc the new grid string with the appropriate number of characters
    populate the string with solid rock characters, except where there are newlines in the original grid string
    set currentlyVisibleGrid to this newly formed grid
  iterate over all coordinates in the currentlyVisibleGrid
    if the point is visible from the player's current location (using isVisible)
      set the character there to whatever character is in the master grid
    else if the point is not solid rock aka the point has been "seen before"
      set the character there to whatever the baseChar is in the master grid
    else do nothing; the character at this place is solidRock and it remains solid rock (neither visible nor seen before)
  in the currently visible grid, set the character at the coordinates of the player to the player character '@' (or whatever `mapchars_player` is)
  return currentlyVisibleGrid
     

#### `grid_findRandomSpawnPosition`
to avoid an infinite loop we only do the random search a limited number of times. After that we locate all 
possible free room spots and choose one of them.

  if any of the passed in pointers is NULL, return false
  for a limited number of times
    choose a random coordinate
    if that coordinate is a room spot
      dereference the x and y pointers and set their value to this coordinate
      return true
  if the spot has not been found yet (which is unlikely but possible)
  initialize an array to the length of the grid string (because that is an upper limit on the number of room spots)
  iterate over the grid and store all the room spots indices in the array
  if the number of room spots found is zero then return false
  otherwise, randomly choose one of the indices found
  return true

#### `grid_addPlayer`
This function only adds the player character to the grid. 
The game and the players within it need to update their state separately.

  if the grid is NULL
    return false
  if the passed in coordinates are invalid
    return false
  if the character at the passed in coordinates is not a room spot
    return false
  otherwise, set the character at that index in the string to the player's letter
  set the playerStandingOn to a room spot
  return false

#### `grid_movePlayer`
Just like the above function, the game and player structs within it need to update their state separately.
This function only moves the player character inside the grid, keeping grid self-consistent.

The return values for this function have a few roles
- -1 means the operation failed
- -2 also means the operation failed, but that there is another player at the spot-to-be-moved to. This must first be resolved through a `grid_swapPlayers`
- otherwise, the operation succeeded and the return value is the amount of gold collected by the player
  
  if grid is NULL or the string within it is null, return -1
  if the amount to move by for each coordinate is not -1, 0, or 1, return -1
  if the new coordinates are not valid, return -1
  if the character at the new coordinate is one of the blocking characters, return -1
  if the character at the new coordinateis not one of the move-to-able characters
    return -2, because this now means there is another player there
  now, finally, we can actually do the move
  if there is gold at the new spot
    call `grid_collectGold` on that spot
  store what the player is curently standing on temporarily
  set the playerStandingOn to whatever is at the new index
  set the character at the new index to the player character
  set the character at the old index to what the player was originally standing on
  return the amount of gold collected, as returned by `grid_collectGold`

#### `grid_swapPlayer`
Same note about grid vs game state as the previous one.

  if grid is NULL or any of the passed in coordinates are invalid
    return
  get the player characters by indexing the given coordinates
  swap the characters at those indices at the grid string
  swap the playerStandingOn characters for both the players


#### `grid_removePlayer`
Same note as the previous one.

  if grid is NULL or the passed in coordinates are invalid
    return false
  set the character at the index corresponding to the given coordinates to whatever the player was standing on
  return true

#### `grid_collectGold`
Same note as the previous one. This just updates the grid's tracking of its own
gold, and doesn't change (or even know about) the game state or the player
purse.
  
  if grid is NULL or the nuggets counterset is NULL or the passed in coordinates are invalid
    return 0
  set int gold to the amount of gold at the given location
  set the counterset value of the gold at the given location to 0
  return gold

#### `grid_getDisplay`

  if grid is NULL return NULL
  create a new string, mallocing just enough spae to store the grid string (which we can calculate from rows and columns)
  copy the grid string into the new string
  return the new string

#### `grid_toMap`

  if grid is NULL or the file pointer is NULL
    return
  otherwise, get the display string from getDisplay
  fputs the string to the file
  free the string

#### `indexOf`
See explanatory comment in the file for a derivation of the formula

  return (numcols + 1) * y + x

#### `getCoordsFromIndex`
Does the exact opposite of numcols. Note that integer division truncates
towards floor.

  if any of the passed in pointers is NULL, return
  otherwise,
  set x to index % (numcols + 1)
  set y to index / (numcols + 1)

#### `isValidCoordinate`
- x and y should both be greater than zero
- x should be less than the number of columns
- y should be less than the number of rows

  return x is positive AND y is positive AND x < numcols AND y < numrows

#### `getPlayerStandingOn`
The hashtable must be keys by a string so for any character 'x', we create
the string ['x', '\0']
  
  if grid is NULL
    return roomSpot (default value)
  create a string to store the letter 
  key into the hashtable and free the string just created
  if the return value is NULL
    return the roomSpot
  otherwise, return whatever the hashtable returned

#### `setPlayerStandingOn`
We can't insert a character, we need to insert a pointer to one.
If the player is new, then we malloc space for a character and insert a pointer
to that into the hashtable. If the player isn't new, then we retrieve the pointer,
dereference it, and update the value of the character stored at that memory location.

  if grid is NULL
    return
  create the key string for the character, as in the last function
  get the pointer at that key
  if it is NULL
    malloc some space for a new character
    set the pointer to the return value of that malloc
    insert tht pointer into the hashtable with the key string
  dereference the pointer and set the value of the character to the new character
  free the key string

#### `isVisible`

  if grid is NULL
    return false
  if isBlockedHorizontally or isBlockedVertically
    return false
  return true

#### `isBlockedHorizontally`
Does the visibility check for all the x-values between px and x (both exclusive).
What this means is that for each of those x-values (call it xi),
it checks where the line segment joining (px, py)-(x,y) intersects the
line x=xi (let's call it yi). If yi is an integer i.e. the line intersects
at a grid point, it checks whether that point blocks vision. Otherwise, the 
line is in the middle of two gridpoints, in which case it checks whether BOTH
of those gridpoints block vision.

This implementation avoids floating point math, only using integer division.
The slope of the line segment is (py - y) / (px - x), which are stored 
separately as slopeNumerator and slopeDenominator. yi = slope * delta + py,
where delta = xi - px. To check if yi is an integer, we check whether
(slopeNumerator * delta) % slopeDenominator is 0. If it is, we only check
(xi, yi) for blocking. Otherwise, we know that yi is the smaller of the two
integers surrounding the 'true' value of yi (because integer division rounds down).
Thus, we check (xi, yi) and (xi, yi + 1)

  if px == x, then there are no x-values to check (this is to prevent slopeDenominator from being 0)
  otherwise, store a sign to know whether to increment or decrement when going from px to x
  store slope numerator and slope denominator 
  go from px to x
    find yi according to the formula described above
    if gridpoint
      if that gridpoint is blocking, return true
    else
      if (xi, yi) and (xi, yi + 1) are both blocking, return true
  return false

#### `isBlockedVertically`
Does exactly the same thing as the above function, but with x and y interchanged.

#### `isBlocking`
returns whether a character blocks line-of-sight

  get the character at the given location
  if the character is a boundary or solid rock or a passage spot
    return true
  return false

#### `freeCharItemDelete`
An itemdelete to delete the hashtable. Deletes a pointer a to a character

  if the passed in pointer is not NULL
    free the passed in pointer

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
   Note: Caller should not delete the grid; it's managed by `game_delete`

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
Grid will be unit tested by a simple C drivers that is just going to perform
some grid actions and print out the resulting grid to stdout or a logfile. 
Since the grid is a string, this does not require any displaying 
program to work.

The testing is handled by `gridtest.c` and `visibilitytest.c`.

#### Game

Because Game is essential to the entire system, we test game whenever we test client and server. These conceptual links and dependencies allow us to debug Game while running server.

#### Player

Because Player is essential to the entire system, we test game whenever we test client and server. These conceptual links and dependencies allow us to debug Player while running server.

### integration testing

#### Server

#### Client
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
