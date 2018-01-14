//**************************************************************************************************
// File:   chord_key_set.h
// Author: James Williamson
// Date:   9/23/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Provides a light-weight, application-specific set data structure for housing integer keys in
// Chord nodes. A 64-bit integer is used for each node, and each bit in the integer corresponds
// to a key (1 for "has key", 0 for "no key"). Operations are provided to manipulate the data
// structure accordingly.
//
// If invalid arguments are passed to any routine, no action is taken.
// 
//**************************************************************************************************

#ifndef CHORD_KEY_SET_H
#define	CHORD_KEY_SET_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stdbool.h>
#include <stdint.h>


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// Chord key type
typedef uint8_t chord_key_t;


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: keyset_init
 * 
 * Initializes the key set, setting the total owned keys to zero.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void keyset_init();


/***************************************************************************************************
 * Function: keyset_add
 * 
 * Add a key to the set. If the key is already in the set, the corresponding bit index is just
 * set again; the set will still only show a single key.
 * 
 * param:  The specified key to add
 * return: void
 **************************************************************************************************/
void keyset_add( chord_key_t key );


/***************************************************************************************************
 * Function: keyset_remove
 * 
 * Remove the key from the set. If the key is not in the set, the corresponding bit index is just
 * cleared again.
 * 
 * param:  The specified key to remove
 * return: void
 **************************************************************************************************/
void keyset_remove( chord_key_t key );


/***************************************************************************************************
 * Function: keyset_check
 * 
 * Check to see if the key is in the set.
 * 
 * param:  The specified key to check for
 * return: True if the key is in the set, false otherwise
 **************************************************************************************************/
bool keyset_check( chord_key_t key );


/***************************************************************************************************
 * Function: keyset_print
 * 
 * Print the content of the key set to the standard output.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void keyset_print();


#endif

//**************************************************************************************************
// End of file
//**************************************************************************************************
