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

/**************** local types ****************/
/*
 * currClient will hold all the relevant info for what we need to use or display to the user
 * ie. their ID, their, currentGold, their port, rows, cols, and spectator
*/
typedef struct clientData {
  // player's characteristics
  char id[2];
  int goldTotal;
  int port;

  // the size of the window
  char rows[5];
  char cols[5];
  bool spectator;
  

} clientData_t;





static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static int parseArgs(const int argc, char* argv[], clientData_t* cData);
static void initializeTerminal(void);
int
main(const int argc, char* argv[])
{

  // check if we can initialize the message
  if (message_init(NULL) == 0) {
    fprintf(stderr, "Error: could not initialize message log");
    return 1; 
  }

  clientData_t cData;
  // parse arguments to create the client
  int errorParseArgs = parseArgs(argc, argv, &cData);

  if (errorParseArgs != 0){
    return errorParseArgs;
  }


  // initializes a screen
  initializeTerminal();

  // set up arguments after parsing
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];

  addr_t server;
  if (!message_setAddr(serverHost, serverPort, &server)) {
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    return 4; // bad hostname/port
  }

  printf("Please send your mesages below:\n");
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


  
  // if only three arguments- send a SPEC message (for spectator)
  if (argc == 3){
    
    // malloc space for a message
    char* message = malloc(sizeof(char) * 9);

    // print into the message SPECTATE
    sprintf(message, "SPECTATE");

    // flag the cData struct to turn on spectator
    cData->spectator = true; 

    // send the message
    message_send(server, message);

    // free the message
    free(message);
    
  }

  // if 4 arguments, the last is the playerNae
  if (argc == 4){

    // retrive the playerName
    char* playerName = argv[3];

    // malloc space for the message
    char* message = malloc(sizeof(char) * (strlen("PlAY ") + strlen(playerName) + 1));

    // print into the message
    sprintf(message, "PLAY %s", playerName);
    cData->spectator = true; 
    message_send(server, message);
    free(message);
    
  }

  return 0;

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
  char c;

  // initialize valid chars to send
  const char* validChars = "hljkuybnHLJKUYBN";
  
  const int messageSize = 12;
  // read a char

  printf("Reading characters");
  if ((c = getch()) != 'q') { 

    // create array to hold message to be send
    char message[messageSize];
    if (strchr(validChars, c)) {
        snprintf(message, sizeof(message), "KEY %c", c);
    } 
    else if (c == 'q') {
        snprintf(message, sizeof(message), "QUIT");
    } 
    else {
        return false;
}

    // Format the message to include the character pressed
    sprintf(message, "KEY %c", c);

    // send the message to the server
    message_send(*server, message);


    // return false - to indicate to keep going
    return false;
  } 
  else {
    // close ncurses
    endwin();
    // return true to stop looping
    return true;
  }
}

/**************** handleMessage() ****************/
static bool handleMessage(void* arg, const addr_t from, const char* message) {
  if(message == NULL) {
    fprintf(stderr, "ERROR message is NULL\n");
    return false;
  }
  refresh();
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