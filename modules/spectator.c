/*
 * This is spectator.h for spectator.c file
 * 
 * A 'spectator' is a struct to represent a spectator in a 'nuggets' game.
 * The struct keeps track of all information that defines a spectator. 
 *
 * Author: Tayeb Mohammadi. COSC 50, Winter, 2024
 * Modified: Ribhu Hooja, March 2024
 * 
 */

#include <stdlib.h>
#include "mem.h"
#include "message.h"
#include "log.h"
#include "spectator.h"

/****************** the spectator type *******************/

typedef struct spectator {
  addr_t address;
} spectator_t;


// to make a new soectator. Check spectator.h for more information 
spectator_t* spectator_new(addr_t address){
    spectator_t* spectator = mem_malloc_assert(sizeof(spectator_t), "Failed to allocte memory for spectator.\n");
    spectator->address = address;
    return spectator;
}

// to get the address of the spectator. Check spectator.h for more information 
addr_t spectator_getAddress(spectator_t* spectator){
    if (spectator != NULL){
        return spectator->address;
    }
    flog_v(stderr, "cannot get the address of null spectator.\n");
    return message_noAddr();
}

//  to delete the the spectaor. Check spectator.h for more information 
void spectator_delete(spectator_t* spectator){
    if(spectator != NULL){
        mem_free(spectator);
    }
    flog_v(stderr, "cannot get the delete of null spectator.\n");
}

// to send message to a spectator. Check spectator.h for more information 
void player_sendMessage(spectator_t* spectator, char* message){
    if(spectator == NULL){
        flog_v(stderr, "Cannot send message for null spectator.\n");
        return;
    }
    message_send(spectator->address, message);
}
