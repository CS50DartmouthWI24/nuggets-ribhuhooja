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

The clientData struct below is used to store all the data that is needed at any point in the client program in one encapsulated place. For instance, the size of the grid that the client is playing on is stored within this struct and then in methods such as `handleGRID` these values are updated and used to check the terminal size. Really, it is create out of a constraint of the message module, which is abstracted so that the specifics of what it is printing can not be specified inside. Additionally, this struct is initialized as a global type because for some of the message modules, there is no `arg` parameter space to pass it in.

```c
typedef struct clientData {
  // player's characteristics
  char id;
  int purse;
  int port;
  int nuggets;

  // the size of the window
  int rows;
  int cols;
  bool spectator;
  
} clientData_t;
```

### Definition of function prototypes

A function to handle the input recieved from stdin (typically the keyboard) and send keystrokes to the server. Called within the message loop.

```c
static bool handleInput(void* arg);
```

A function to handle the messages recieved from the server and display, update, begin, or quit the client's interface accordingly.

```c
static bool handleMessage(void* arg, const addr_t from, const char* message);
```

A function to parse the command-line arguments and verify their validitiy, and send a message to the server if arguments verified to start a spectator or player in the game.

```c
static int parseArgs(const int argc, char* argv[], clientData_t* cData);
```

A function to initialize the ncurses terminal, without echoing, and handle keystrokes continuously.

```c
static void initializeTerminal(void);
```

A function to handle a GRID message as sent from the server. Prompts user to resize if neccesary.

```c
static void handleGRID(const char* message, void* arg);
```

A function to handle a GOLD message as sent from the server. Displays the status bar with the updated gold counts.

```c
static void handleGOLD(const char* message, void* arg);
```

A function to handle a QUIT message as sent from the server. Quits the game and ends the ncurses window.

```c
static void handleQUIT(const char* message, void *arg);
```

A function to handle an ERROR messsage as sent from the server. Logs the error sent to stderr (done autonomously by the `message` and `log` modules)

```c
static void handleERROR(const char* message);
```

A function to handle a DISPLAY message as sent from the server. Displays the grid to the player that has been sent from the server, with the correct formatting

```c
static void handleDISPLAY(const char* message, void* arg);
```

A function to handle an OK message as sent from the server. Initialzes the playerID that will be stored and kept track of in client for displaying

```c
static void handleOK(const char* message, void* arg);
```

### Detailed pseudo code

#### `handleInput`:
	parse the arg to a server
	read in a keystroke from stdin
	if key is not Q
		send a message to server in form of KEY [key]
		return false
	else
		send KEY Q to the server
		end the window
		return true

#### `handleMessage`:
	if NULL message
		print to stderr error message
		return false
	else if GRID message
		call handleGRID
	else if QUIT message
		call handleQUIT
	else if GOLD message
		call handleGOLD
	else if DISPLAY message
		call handleDISPLAY
	else if ERROR message
		call handleERROR
	else if OK message
		call handleOK
	else
		refresh the screen
		print an error message to the screen
	refresh()
	return false
		
#### `parseArgs`:
	if not 3 or 4 args passed
		print message to stderr
		exit non-zero
	read server host and server port form commandline
	set up a server from serverHost and serverPort and check initialization 
	if 3 arguments
		send a SPECTATE message to the server
		set cData spectator to true
	if 4 arguments
		send a PLAY [playerName] message to the server where [playerName] is the 4th command line argument
		set cData spectator to false
	return 0 on success with above

#### `initializeTerminal`:
	initialize screen
	turn off the character break
	set no echos
	refresh

#### `handleGRID`:
	cast the arg to a cData type
	read in the rows and cols of the GRID r c message
	store those values in the cData stores
	get the current terminal size
	while that size is too small
		prompt user to increase size
		wait for them to type a char
		reread in size after they type anything
		clear and refresh screen
	refresh the screen

#### `handleGOLD`:
	cast arg to cData type
	read in the GOLD n p r message to a 
	store the purse and nuggets read in to the cData
	if spectator
		print status of specator on the top line (only showing unclaimed nuggets)
		refresh
	else if player but no nuggets claimed
		print status on top with ID, purse and unclaimed
		refresh
	else
		print status with ID, purse, unclaimed, and nuggets just picked up
		refresh


#### `handleQUIT`:
	copy message to edit
	skip prefix by incrementing pointer
	clear the screen, end ncurses
	print the message to stdout
	exit zero

#### `handleERROR`:
	(logging is taken care of by message module so don't do anything)

#### `handleDISPLAY`:
	cast arg to cData
	copyMessage to edit
	increment pointer to get rid of prefix
	extract cols in the grid
	set up y=1 (to start on second row)x=0 (track x position), j=0 (track cols in grid)
	for int i =0; map[i] != '\0'; i++
		if map[i] == '\n' OR j >= cols
			y++
			x=0
			j=0
		else
			move to y and x spot and print map[i] there
			increment x and increment j
		if map[i] == '\n'
			j=0
	refresh

#### `handleOK`:
	cast cData to arg
	read in OK message to get char sent by server
	store the char in cData

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

#### `getPlayerStandingOn`

#### `setPlayerStandingOn`

#### `isVisible`

#### `isBlockedHorizontally`

#### `isBlockedVertically`

#### `isBlocking`

#### `freeCharItemDelete`

## Game

### Data structures

### Definition of function prototypes

### Detailed pseudo code

## Player

### Data structures

### Definition of function prototypes

### Detailed pseudo code

## Spectator

### Data structures

### Definition of function prototypes


### Detailed pseudo code

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

#### Player

### integration testing

#### Server

#### Client

### system testing

> For teams of 4: How will you test your client and server together?

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
