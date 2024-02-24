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


static bool handleInput(void* arg);
static bool handleMessage(void* arg, const addr_t from, const char* message);


int
main(const int argc, char* argv[])
{

  // check if we can initialize the message
  if (message_init(NULL) == 0) {
    fprintf(stderr, "Error: could not initialize message log");
    return 1; 
  }

  const char* program = argv[0];
  if (argc != 3) {
    fprintf(stderr, "usage: %s hostname port\n", program);
    return 2; // bad commandline
  }


  // initializes a screen
  newterm(NULL, stdout, stdin);
  cbreak(); 
  noecho();
  refresh();
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


/**************** handleInput ****************/
static bool handleInput(void* arg) {

  // parse the server address
  addr_t* server = arg;
  char c;

  // read a char
  if ((c = getch()) != 'q') { 

    // malloc space for a string
    char* message = (char *) malloc(6 * sizeof(char));
    if (message == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return true; // Error handling
    }

    // Format the message to include the character pressed
    sprintf(message, "KEY %c", c);

    // send the message to the server
    message_send(*server, message);

    // free the message
    free(message);

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