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
#include "client.h"
#include "../support/log.h"
#include "../support/message.h"
#include <ncurses.h>
#include <unistd.h>

/**************** local types ****************/
/*
 * currClient will hold all the relevant info for what we need to use or display to the user
 * ie. their ID, their, currentGold, their port, rows, cols, and spectator
*/
typedef struct clientData {
  // player's characteristics
  char id[2];
  int purse;
  int port;
  int nuggets;

  // the size of the window
  int rows;
  int cols;
  bool spectator;
  

} clientData_t;

static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static int parseArgs(const int argc, char* argv[], clientData_t* cData);
static void initializeTerminal(void);
static void handleGRID(const char* message, void* arg);
static void handleGOLD(const char* message, void* arg);
static void handleQUIT(const char* message, void *arg);
static void handleERROR(const char* message);
static void handleDISPLAY(const char* message, void* arg);

// global client data since can't pass through
clientData_t cData; // set up client data

int
main(const int argc, char* argv[])
{
  if (message_init(NULL) == 0) { // check if we can initialize the message
    fprintf(stderr, "Error: could not initialize message log");
    return 1; 
  }
 
  int errorParseArgs = parseArgs(argc, argv, &cData); // parse arguments
  if (errorParseArgs != 0){
    return errorParseArgs;
  }
  
  initializeTerminal(); // initializes a screen

  // set up arguments after parsing
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];
  (&cData)->port = atoi(argv[2]);
  addr_t server; 


  char* grid = "GRID 3 4";
  handleGRID(grid, &cData);

  char* display = "DISPLAY\naaaa\nbbbb\nccccc";
  handleDISPLAY(display, &cData);

  // create server address
  if (!message_setAddr(serverHost, serverPort, &server)) { 
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    return 4; // bad hostname/port
  }

  // listen for and send messages
  bool ok = message_loop(&server, 0, NULL, handleInput, handleMessage);

  // shut down the message module
  message_done();


  return ok? 0 : 1; // status code depends on result of message_loop

}

/**************** parseArgs ****************/
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
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
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

/**************** handleInput ****************/
static bool handleInput(void* arg) {
  // parse the server address
  addr_t* server = arg;

  // check if valid
  if (server == NULL) {
    fprintf(stderr, "Error: invalid server address");
    return true;
  }

  // initialze char
  char key;

  // initialize valid chars to send
  const char* validChars = "hljkuybnHLJKUYBN";
  const int messageSize = 12;


  if ((key = getch()) != 'q') { 

    char message[messageSize];
    if (strchr(validChars, key)) {

      snprintf(message, sizeof(message), "KEY %c", key);

    } 
    else if (key == 'q') {

      snprintf(message, sizeof(message), "QUIT");

    } 
    else {
      return false;
}
  sprintf(message, "KEY %c", key);
  message_send(*server, message);
  return false;

  } 
  else {
    endwin();
    return true;
  }
}

/**************** handleMessage() ****************/
static bool handleMessage(void* arg, const addr_t from, const char* message) {


  if(message == NULL) {
      fprintf(stderr, "ERROR message is invalid \n");
      return false;
  }

  if(strncmp(message, "GRID ", strlen("GRID ")) == 0) { 
    handleGRID(message, &cData);
  } 
  if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
    handleQUIT(message, &cData);
  } 
  else if(strncmp(message, "GOLD ", strlen("GOLD ")) == 0) { 
    handleGOLD(message, &cData);
  } 
  else if(strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
    handleDISPLAY(message, &cData);
  } 
  else if(strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
    handleERROR(message);
  } 
  else {
      printf("Error: passed in an erroneous message.\n");
      refresh();
  }
  return false;
}

/**************** initializeTerminal() ****************/
static void initializeTerminal(void){
  // initialize the screen, no waiting for newline, no echoing, yes keypad navigation
  initscr(); 
  cbreak(); 
  noecho(); 
  refresh(); 
}

/**************** handleGRID() ****************/
static void handleGRID(const char* message, void* arg){

  // cast the client data to an argument
  clientData_t* cData = (clientData_t*) arg;

  // initialize data to read in
  int rows;
  int cols;

  sscanf(message, "GRID %d %d", &rows, &cols); // read in data from message


  int nrows;
  int ncols;

  getmaxyx(stdscr, nrows, ncols);

  cData->rows = rows;
  cData->cols = cols;

  while (nrows < rows || ncols < cols){
    printw("Your window must be at least %d high and %d wide \n", rows, cols);
    printw("Resize your window then press 1 when resized. \n");

    if (getch() == '1') {
      getmaxyx(stdscr, nrows, ncols);
    }
    
    // refresh screen
    clear();
    refresh();
  }
  
  refresh();
}

/**************** handleGOLD() ****************/
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
    mvprintw(0,0, "Spectator: %d nuggets unclaimed. Play at plank %d\n", remaining, cData->port);
    refresh();  

  }
  else if (cData->nuggets == 0) {  
    move(0,0);
    mvprintw(0,0, "Player %s has %d nuggets (%d nuggets unclaimed).\n", cData->id, purse, remaining);
    refresh();  
  }
  else {
    move(0,0);
    mvprintw(0,0, "Player %s has %d nuggets (%d nuggets unclaimed). GOLD received: %d\n", cData->id, purse, remaining, nuggets);
    refresh();
  } 
}

/**************** handleQUIT() ****************/
static void handleQUIT(const char* message, void *arg){
  // end the game
  nocbreak();
  endwin();
  exit(0);
}

/**************** handleERROR() ****************/
static void handleERROR(const char* message){
  
}


/**************** handleDISPLAY() ****************/
static void handleDISPLAY(const char* message, void* arg){

  clientData_t* cData = (clientData_t*) arg;

  // copy message to edit
  char* messageCopy = malloc(strlen(message) + 1); 
  strcpy(messageCopy, message);

  // get the map by incrementing  the pointer
  char* map = messageCopy + strlen("DISPLAY\n");  

  // varaiables for looping
  int cols = cData->cols;
  int currY = 0; 
	int currX = 0; 
	int j = 0; 

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

  // free map
  // free(map);

}

