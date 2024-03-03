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
