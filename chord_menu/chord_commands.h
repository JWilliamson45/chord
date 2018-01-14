//**************************************************************************************************
// File:   chord_commands.h
// Author: James Williamson
// Date:   9/25/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Contains routines to build and send Chord commands to the DHT node ring.
// 
//**************************************************************************************************

#ifndef CHORD_COMMANDS_H
#define	CHORD_COMMANDS_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

#include "chord_error.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: cmd_create_main_node
 * 
 * Create the first node in the DHT ring by using a fork-exec to run the chord_node program.
 * 
 * param:  void
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t cmd_create_main_node();


/***************************************************************************************************
 * Function: cmd_add_node
 * 
 * Command to add a new node to the DHT ring. If the maximum amount of supported nodes are already
 * created, no action is taken and an error code is returned. The new node ID is randomly
 * generated; this function will block until an unassigned ID is found.
 * 
 * param:  void
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t cmd_add_node();


/***************************************************************************************************
 * Function: cmd_add_key
 * 
 * Command to add a new key to the DHT ring. If the key is already present in the DHT, an error
 * is returned; it will not be added again.
 * 
 * param:  the key to be added to the DHT
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t cmd_add_key( int key_id );


/***************************************************************************************************
 * Function: cmd_delete_key
 * 
 * Command to delete a key from the DHT ring. If the key is not present in the DHT, an error
 * is returned.
 * 
 * param:  the key to be removed from the DHT
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t cmd_delete_key( int key_id );


/***************************************************************************************************
 * Function: cmd_dump
 * 
 * Command to have all nodes in the DHT dump their ID and key set to standard output.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void cmd_dump();


/***************************************************************************************************
 * Function: cmd_toggle_debug
 * 
 * Command to toggle the output of debug messages. This is useful for development purposes. By 
 * default, debug is off.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void cmd_toggle_debug();


#endif

//**************************************************************************************************
// End of file
//**************************************************************************************************
