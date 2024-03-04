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

The server module does not create any new structures.

### Definition of function prototypes

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```

A function that will be called in `message_loop()` to handle incoming messages from client.

```c
static bool handleMessage(void* arg, const addr_t from, const char* message);
```

A function that will be called when the PLAY message is sent to handle the PLAY functionality.

```c
static void handlePlay(void* arg, const addr_t from, const char* content);
```

A function that will be called when the KEY message is sent to handle the KEY functionality.

```c
static bool handleKey(void* arg, const addr_t from, const char* content);
```

A function that will be called when the SPECTATE message is sent to handle the SPECTATE functionality.

```c
static void handleSpectate(void* arg, const addr_t from, const char* content);
```

A function to handle the client quitting.

```c
static void keyQ(const addr_t from);
```

A function to generate an error message. Will be called when an incorrect key is pressed.

```c
static void errorMessage(const addr_t from, const char* content);
```

A function to check if a given string is empty.

```c
static bool checkWhitespace(const char* name)
```

A function to normalize a given string to match the requirements of a player name.

```c
static char* fixName(const char* entry)
```

### Detailed pseudo code

#### `main`:

	create map and seed variables
	parseArgs
	intialize game
	if game == NULL
		exit with non-zero error code
	close map file
	initialize messages, load port
	if port == 0
		return 2
	else
		print port
	loop through messages
	close messages
	return 1 or 2 depending on success of messages loop

#### `parseArgs`:

	if 2 or 3 args given
		check if map file can be opened for reading
		if 3 args are given
			make sure its a valid seed number
			seed the random-number generator with the given seed
		else
			seed the random-number generator with getpid()
	else
		print correct usage

#### `handleMessage`:

	gameOver = false
	if message starts with PLAY
		save content of message
		handlePlay()
	if message starts with KEY
		save content of message
		handleKey()
	if message starts with SPECTATE
		save content of message
		handleSpectate()
	return gameOver

#### `handlePlay`:

	if string isn't empty
		if game isn't at full capacity
			find random spawn location for player
			calculate player letter
			normalize player name
			create new player
			add player to the game
			create OK message
			send OK message
			find nrows
			find ncols
			create GRID message
			send GRID message
			find amount of gold
			create GOLD message
			send GOLD message
			find relevant display
			craete DISPLAY message
			send DISPLAY message
			if game has a spectator
				get spectator address
				get the master display
				create DISPLAY message
				send DISPLAY message
		else
			send QUIT message
	else
		send QUIT message

#### `handleKey`:

	gameOver = false
	if string passed is valid
		switch letter
			case Q: keyQ()
			case q: keyQ()
			case h: move left
			case l: move right
			case j: move down
			case k: move up
			case y: move up and left
			case u: move up and right
			case b: move down and left
			case n: move down and right
			case H: long move left
			case L: long move right
			case J: long move down
			case K: long move up
			case Y: long move up and left
			case U: long move up and right
			case B: long move down and left
			case N: long move down and right
			default: errorMessage()
	return gameOver

#### `handleSpectate`:

	add spectator to game
	find nrows
	find ncols
	create GRID message
	send GRID message
	find gold amount
	create GOLD message
	send GOLD message
	find master display
	create DISPLAY message
	send DISPLAY message

#### `keyQ`:

	find player from address
	if play doesn't exist (its a spectator)
		remove spectator from game
	else
		remove player from game

#### `errorMessage`:

	create ERROR message
	send ERROR message

#### `checkWhitespace`:

	for each character in given string
		if its a letter in the alphabet
			return false
	return true

#### `fixName`:

	create empty name string
	if given string is less than max length
		for each letter
			if !isgraph and !isblank
				make that index in name an underscore
			else
				make that index in name the same letter
		add '\0' at end
	else
		for each letter
			if !isgraph and !isblank
				make that index in name an underscore
			else
				make that index in name the same letter
		add '\0' at end
	return name

---

## Grid

### Data structures

### Definition of function prototypes

### Detailed pseudo code

## Game

### Data structures

### Definition of function prototypes

### Detailed pseudo code

## Player

### Data structures

### Definition of function prototypes

### Detailed pseudo code

---

## Testing plan

### unit testing
#### Grid
Grid will be unit tested by a simple C driver that is just going to perform
some grid actions and print out the resulting grid to stdout. Since the grid
is a string, this does not require any displaying program to work.

#### Game


#### Player

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

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
