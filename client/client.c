/*
 * client.c - program to join a 'nuggets' game as a client
 * 
 * Paul Cherian, COSC 50, Febuary 2024
 * 
 */ 

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../support/log.h"
#include "../support/message.h"
#include <ncurses.h>
#include <unistd.h>


/************ struct clientData **************/
/* wraps all the neccesary data that a client should 
 * be able to access in one struct */
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


// function prototypes 
static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static int parseArgs(const int argc, char* argv[], clientData_t* cData);
static void initializeTerminal(void);
static void handleGRID(const char* message, void* arg);
static void handleGOLD(const char* message, void* arg);
static void handleQUIT(const char* message, void *arg);
static void handleERROR(const char* message);
static void handleDISPLAY(const char* message, void* arg);
static void handleOK(const char* message, void* arg);


// global client data since can't pass specify arg to pass in messages
clientData_t cData; // set up client data



/***************** main() *****************/ 
/* 
 * Caller provides: 
 *  keystrokes from keyboard
 * We do: 
 *  run the main logic of the game
 * We return:
 *  return 0 or 1 depending on success of exectution
 */
int
main(const int argc, char* argv[])
{

  // initialize the message and extract the port 
  int ourPort = message_init(stderr);

  // check if that was successful
  if (ourPort == 0) {
    // print to stderr log if not
    fprintf(stderr, "Error: could not initialize message log");
    return 1;  // return failure
  }

  // initialize the start of the log
  log_v("START OF LOG\n");
 
  // parse arguments and check if successful
  int errorParseArgs = parseArgs(argc, argv, &cData); 
  if (errorParseArgs != 0){
    return errorParseArgs;
  }
  
  initializeTerminal(); // initializes a screen

  // set up arguments after parsing
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];
  (&cData)->port = atoi(argv[2]);
  addr_t server; 

  // populate the server addr_t by using the host and portname
  if (!message_setAddr(serverHost, serverPort, &server)) { 
    // fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    return 4; // bad hostname/port
  }

  // listen for and send messages
  bool ok = message_loop(&server, 0, NULL, handleInput, handleMessage);

  // shut down the message module
  message_done();

  return ok? 0 : 1; // status code depends on result of message_loop

}

/***************** parseArgs() *****************/ 
/* 
 * Caller provides: 
 *  command line arguments and client data object to read into
 * We do: 
 *  parse the arguments and store data into the client data object
 * We return:
 *  0 on valid parameters, anything else means invalid parameters
 */
static int parseArgs(const int argc, char* argv[], clientData_t* cData){

  // check numArgs passed

  if (argc != 3 && argc != 4) {
    fprintf(stderr, "Error: improper number of arguments specified\n");
    return 2;
  }

  // get serverHost and serverPort
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];

  // set up an addr_t for the server
  addr_t server; // address of the server
  if (!message_setAddr(serverHost, serverPort, &server)) {
    return 3; // bad hostname/port
  }
  
  if (argc == 3){ // if only three arguments- spectator
    
    char* message = malloc(sizeof(char) * 9); // malloc space for message
    sprintf(message, "SPECTATE"); // print into the message SPECTATE
    cData->spectator = true; // flag the cData struct to turn on spectator
    message_send(server, message); // send the message
    free(message); // free the message

  }

  
  if (argc == 4){ // if 4 arguments, the last is the playerName

    char* playerName = argv[3]; // retrive the playerName
    char* message = malloc(sizeof(char) * 
    (strlen("PlAY ") + strlen(playerName) + 1)); // malloc space for message
    sprintf(message, "PLAY %s", playerName); // print into the message
    cData->spectator = false; // turn off spectator
    message_send(server, message); // send message to server
    free(message); // free the message
    
  }

  return 0; // return success

}

/***************** handleInput() *****************/ 
/* 
 * Caller provides: 
 *  the server as an arg parameter
 * We do: 
 *  read in keystrokes from stdin and send them to server
 * We return:
 *  bool to indicate whether or not to keep handlingInput
 *  true- stop, false- continue
 */
static bool handleInput(void* arg) {

  // parse the server address
  addr_t* server = arg;

  // check if server is not null
  if (server == NULL) {
    fprintf(stderr, "Error: server argument is NULL\n");
    return true;
  }

  // check if server looks like an address
  if (!message_isAddr(*server)) {
    fprintf(stderr, "Error: server passed in is not a valid address\n");
    return true;
  }

  // initialze char and set buffersize of 12
  char key;
  const int messageSize = 12;

  // read a key from stdin, if not q
  key = getch();
  if (key != 'Q' && key != EOF) { 

    // create a static char array to print into
    char message[messageSize];
   
    // print into the array
    snprintf(message, sizeof(message), "KEY %c", key);

    // send the message to the server
    message_send(*server, message);

    // return false to indicate keep on looping
    return false;

  } 
  else {
    
    // send q key to the server if client quit
    message_send(*server, "KEY Q");
    
    // close the ncurses
    endwin();
    
    // return true to keep looping
    return true;
  }
}

/***************** handleMessage() *****************/ 
/* 
 * Caller provides: 
 *  addr_t of where where the message has been sent 
 *  from and the message that has been sent to the 
 *  client
 * 
 * We do: 
 *  handle the message accordlingly and update
 *  the client data depending on what it says
 * 
 * We return:
 *  a bool depending on if message loop should continue or not (always should)
 */
static bool handleMessage(void* arg, const addr_t from, const char* message) {

  // check if message exists
  if(message == NULL) {
    fprintf(stderr, "Error: message is NULL\n"); // print to log if not
    return false; // keep listening
  }

  // handle GRID message
  else if(strncmp(message, "GRID ", strlen("GRID ")) == 0) { 
    handleGRID(message, &cData);
  } 

  // handle QUIT message
  else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
    handleQUIT(message, &cData);
  } 

  // handle GOLD message
  else if(strncmp(message, "GOLD ", strlen("GOLD ")) == 0) { 
    handleGOLD(message, &cData);
  } 

  // handle DISPLAY message
  else if(strncmp(message, "DISPLAY", strlen("DISPLAY")) == 0) {
    handleDISPLAY(message, &cData);
  } 

  // handle ERROR message
  else if(strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
    handleERROR(message);
  } 

  // handle OKAY message
  else if(strncmp(message, "OK ", strlen("OK ")) == 0) {
    handleOK(message, &cData);
  } 
  else {

    // refresh the screen to show changes
    refresh();
    
    // print invalid message if incoming message does not adhere to 
    // any of the above conditions, print at bottom row
    mvprintw((&cData)->rows,0, "Error: message does not have known formatting\n");

  }

  refresh(); // refresh the screen to show changes
  return false; // keep listening
}

/***************** initializeTerminal() *****************/ 
/* 
 * Caller provides: 
 *  nothing
 * 
 * We do: 
 *  initialize ncurses with specifications
 * 
 * We return:
 * void
 */
static void initializeTerminal(void){
  // initialize the screen, no waiting for newline, 
  // no echoing, yes keypad navigation
  initscr(); 
  cbreak(); 
  noecho(); 
  refresh(); 
}

/***************** handleGRID() *****************/ 
/* 
 * Caller provides: 
 *  message from the server and cData as arg
 * 
 * We do: 
 *  update the cData struct with row and cols needed for grid in message
 * 
 * We return:
 *  void
 */
static void handleGRID(const char* message, void* arg){

  // cast the client data to an argument
  clientData_t* cData = (clientData_t*) arg;

  // initialize data to read in
  int rows;
  int cols;

  sscanf(message, "GRID %d %d", &rows, &cols); // read in data from message

  // initialize rows
  int nrows;
  int ncols;

  // get current terminal size
  getmaxyx(stdscr, nrows, ncols);

  // store needed size in cData
  cData->rows = rows;
  cData->cols = cols;

  // while the size of the terminal is not larger than 
  // the size of the grid + 1 (so we can display messages in bottom row)
  while (nrows < rows + 1 || ncols < cols + 1){

    // prompt user to increase size
    printw("Window size requirements: %d rows by %d cols\n", rows+1, cols+1);
    printw("Please resize your window and press any key to continue\n");

    // re-get the after the user enters a key
    getch();
    getmaxyx(stdscr, nrows, ncols);
    
    // refresh screen
    clear();
    refresh();
  }
  
  refresh(); // refresh screen to show changes
}

/***************** handleGOLD() *****************/ 
/* 
 * Caller provides: 
 *  message from the server and cData as arg
 * 
 * We do: 
 *  update the gold tracking data in the cData struct according
 *  to the message from the server
 * 
 * We return:
 *  void
 */
static void handleGOLD(const char* message, void* arg){

  clientData_t* cData = (clientData_t*) arg; // cast client from arg

  // initialize data to read in
  int nuggets;
  int purse;
  int remaining;

  // read data from the message
  sscanf(message, "GOLD %d %d %d", &nuggets, &purse, &remaining); 
  cData->purse = purse;
  cData->nuggets = nuggets;

  if (cData->spectator) { // if client is spectator, only print remaining nuggets
    move(0,0);
    // print spectator message about unclaimed nuggets in last row
    mvprintw(0,0, "Spectator: %d nuggets unclaimed", remaining);
    refresh();  

  }
  // if player did not pick up any nuggets on this message
  else if (cData->nuggets == 0) {  
    move(0,0);
    // print player message about current nuggets, and remaining nuggets 
    mvprintw(cData->rows,0, "Player %c has %d nuggets (%d nuggets unclaimed)", cData->id, purse, remaining);
    refresh();  
  }

  // player did pick up nuggets so we have to display to them as well
  else {
    move(0,0);
    // print player message about current nuggets, and remaining nuggets and the GOLD nuggets they recieved
    mvprintw(cData->rows,0, "Player %c has %d nuggets (%d nuggets unclaimed). GOLD received: %d", cData->id, purse, remaining, nuggets);
    refresh();
  } 
}


/***************** handleQUIT() *****************/ 
/* 
 * Caller provides: 
 *  message from the server and cData as arg
 * 
 * We do: 
 *  end the game by shutting down ncurses, then printing
 *  the message to stdout
 * 
 * We return:
 *  void (but we exit success if handleQUIT called)
 */
static void handleQUIT(const char* message, void *arg){

  // copy message to edit
  char* messageCopy = malloc(strlen(message) + 1); 
  strcpy(messageCopy, message);

  // get the map by incrementing the pointer to the message
  // and skipping the prefix
  char* quitMessage = messageCopy + strlen("QUIT ");  

  // end the game
  clear(); // clear the screen
  nocbreak(); // get rid of continuous input
  endwin(); // end the ncurses window
  printf("%s", quitMessage); // print the QUIT message
  free(messageCopy);
  exit(0); // exit successfully
}

/***************** handleERROR() *****************/ 
/* 
 * Caller provides: 
 *  message from the server 
 * 
 * We do: 
 *  nothing (logging is taken care of by the message module)
 *  existence of function is for encapsulation and clarity of 
 *  logic
 * 
 * We return:
 *  void 
 */
static void handleERROR(const char* message){
  // print to last rows
}


/***************** handleDISPLAY() *****************/ 
/* 
 * Caller provides: 
 *  message from the server and cData as arg
 * 
 * We do: 
 *  print the grid in the messsage recieved from the server 
 *  to the display using ncurses
 * 
 * We return:
 *  void 
 */
static void handleDISPLAY(const char* message, void* arg){
  
  // cast the arg to cData
  clientData_t* cData = (clientData_t*) arg;

  // copy message to edit
  char* messageCopy = malloc(strlen(message) + 1); 
  strcpy(messageCopy, message);

  // get the map by incrementing the pointer to the message
  // and skipping the prefix
  char* map = messageCopy + strlen("DISPLAY\n");  

  // varaiables for looping
  int cols = cData->cols; // cols needed for the  grid
  int currY = 0; 
	int currX = 0; 
	int j = 0;  // counter of what column we are at

	// loop through the map
	for (int i = 0; map[i] != '\0'; i++) {
    
    // check if at the end of a line or at the end of screen
		if (map[i] == '\n' || j >= cols) {

			// reset back to left of line, but go one line down
			currY++;
			currX = 0;
			j = 0; // reset cols

		} 
    else {
      
			// add char at the currX and currY and mv there
			mvaddch(currY, currX, map[i]);

      // increment the currentX and the column we are on
			currX++;
			j++;

		}

    // check if at newline, if so reset to start
		if (map[i] == '\n') { 
			j = 0;
		}
	}

	// refresh screen
	refresh();                              

  // free messageCopy
  free(messageCopy);


}

/***************** handleOK() *****************/ 
/* 
 * Caller provides: 
 *  message from the server and cData as arg 
 * 
 * We do: 
 *  stores a correctly formatted ID into the cData struct
 * 
 * We return:
 *  void 
 */
static void handleOK(const char* message, void* arg){

  // cast the client data to an argument
  clientData_t* cData = (clientData_t*) arg;

  // initialize the playerID character
  char playerID;
  sscanf(message, "OK %c", &playerID);

  // store the characterID into 
  cData->id = playerID;

}

