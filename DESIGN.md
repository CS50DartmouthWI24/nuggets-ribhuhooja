# CS50 Nuggets
## Design Spec
### Torpedos, Winter 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes player, grid, and gridpoint modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Player

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.
    
### Inputs and outputs

#### Inputs
The clients will input individual keystrokes (without the need of a newline to send)

See the Requirements spec for the usage of each key

#### Outputs
The output of the program will be a visual, realtime display of the game grid. 
Te program will also display a status line. See the requirements spec for 
the format of the status line.

### Functional decomposition into modules

Our client comprises the following modules:

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

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

The server will run as follows:

	execute from a command line per the requirement spec
	parse the command line, validate parameters
	call initializeGame() to set up data structures
	initialize the 'message' module
	print the port number on which we wait
	call message_loop(), to await clients
	call gameOver() to inform all clients the game has ended
	clean up


> Then briefly describe each of the major functions, perhaps with level-4 #### headers.

### Major data structures

> Describe each major data structure in this program: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.
> Mention, but do not describe, data structures implemented by other modules (such as the new modules you detail below, or any libcs50 data structures you plan to use).

    grid- stores the grid string, a list of player objects,and goldNuggets. It should contain all the information to describe the current game state of nuggets. Additionally, it will have methods to send the grid with the correct amount of information to any player and spectator.
    gridPoint- 
    player-
---

## XYZ module

> Repeat this section for each module that is included in either the client or server.

### Functional decomposition

> List each of the main functions implemented by this module, with a phrase or sentence description of each.

### Pseudo code for logic/algorithmic flow

> For any non-trivial function, add a level-4 #### header and provide tab-indented pseudocode.
> This pseudocode should be independent of the programming language.

### Major data structures

> Describe each major data structure in this module: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.
