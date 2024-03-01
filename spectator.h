/*
 * spectator.h- header file for spectator.c file
 * 
 * A 'spectator' is a struct to represent a spectator in a 'nuggets' game.
 * The struct keeps track of all information that defines a spectator 
 *
 * Paul Cherian, COSC 50, Febuary 2024
 * Updated by Tayeb Mohammadi
 * 
 */

#ifndef SPECTATOR_H
#define SPECTATOR_H

#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "grid.h"

/************* global types *************/
typedef struct spectator {
    addr_t* address;
} spectator_t;

/**** spectator_new ****/
/* Create a new spectator struct to observe nuggets games
 * We do: 
 *  Returns a spectator_t* object with the given address
 *  Returns NULL if any errors
 */
spectator_t* spectator_new(addr_t* address);

/****** spectator_getAddress ******/
/* gets the address of a given spectator
 * Caller provides: 
 *  spectator struct
 * We do: 
 *  Return the spectator struct address. Return NULL on any error
 */
addr_t* spectator_getAddress(spectator_t* spectator);

/****** spectator_delete ****/
/* 
 * Deletes a given spectator
 * Caller provides: 
 *  pointer to spectator struct 
 * We do: 
 *  Delete spectator and free all associated memory
 * We return:
 *  void
 */
void spectator_delete(spectator_t* spectator);

/************* spectator_sendMessage *************/
/* 
 * Send message to a spectator
 * Caller provides: 
 *  A pointer to the spectator and a pointer to message string
 * We do: 
 *  Send the message to spectator
 */
void spectator_sendMessage(spectator_t* spectator, char* message);


#endif // SPECTATOR_H