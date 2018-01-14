//**************************************************************************************************
// File:   chord_node.h
// Author: James Williamson
// Date:   9/26/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Models a Chord node process that is part of the DHT. The program is capable of forking itself
// to create new (local-only) nodes if a command to add a node is received from the menu process.
// A Chord node handles the addition or removal of keys from its local set, the scheme of which 
// is based on its ID and place in the DHT ring.
//
// Each node knows its successor information and local keys, but not the complete DHT node list or
// total keys present in the system. The intent is to decentralize the algorithm.
// 
//**************************************************************************************************

#ifndef CHORD_NODE_H
#define CHORD_NODE_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

#include "chord_message.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: init_dht
 * 
 * Initialize the distributed hash table,
 * 
 * param:  The pipe handle from the menu process, so that commands may be received
 * return: void
 **************************************************************************************************/
void init_dht( int menu_pipe_handle );


/***************************************************************************************************
 * Function: check_messages
 * 
 * Check for incoming messages from other nodes in the DHT.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void check_messages( void );


#endif

//**************************************************************************************************
// End of file.
//**************************************************************************************************
