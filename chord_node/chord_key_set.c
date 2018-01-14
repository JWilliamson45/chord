//**************************************************************************************************
// File:   chord_key_set.c
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

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stdio.h>
#include <string.h>
#include "chord_config.h"
#include "chord_key_set.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// Each 64-bit integer models the actual key set for a node - each bit corresponds to a key
static uint64_t key_set = 0;


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
void keyset_init()
{
    key_set = 0;
}


/***************************************************************************************************
 * Function: keyset_add
 * 
 * Add a key to the set. If the key is already in the set, the corresponding bit index is just
 * set again; the set will still only show a single key.
 * 
 * param:  The specified key to add
 * return: void
 **************************************************************************************************/
void keyset_add( chord_key_t key )
{
    if( key < MAX_KEY_VALUE )
    {
        // Add key to set
        key_set |= ( 1UL << key );
    }
}


/***************************************************************************************************
 * Function: keyset_remove
 * 
 * Remove the key from the set. If the key is not in the set, the corresponding bit index is just
 * cleared again.
 * 
 * param:  The specified key to remove
 * return: void
 **************************************************************************************************/
void keyset_remove( chord_key_t key )
{
    if( key < MAX_KEY_VALUE )
    {
        // Remove key from set
        key_set &= ~( 1UL << key );
    }
}


/***************************************************************************************************
 * Function: keyset_check
 * 
 * Check to see if the key is in the set.
 * 
 * param:  The specified key to check for
 * return: True if the key is in the set, false otherwise
 **************************************************************************************************/
bool keyset_check( chord_key_t key )
{
    bool key_found = false;
    
    if( key < MAX_KEY_VALUE )
    {
        if( key_set & ( 1UL << key ) )
        {
            // Key is present; the corresponding bit is set
            key_found = true;
        }
    }
    
    return( key_found );
}


/***************************************************************************************************
 * Function: keyset_print
 * 
 * Print the content of the key set to the standard output.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void keyset_print()
{
    // Local variables
    char string[512];                 // Holds the fully-assembled string to print
    char converted_number[4];         // Holds a number converted to a string
    int key_index = 0;                // Used to cycle through the keys
    
    // Clear buffer
    memset( string, 0x00, sizeof( string ) );
    
    // Cycle through the set and print any keys that are present
    for( key_index; key_index < MAX_KEY_VALUE; key_index++ )
    {
        if( keyset_check( key_index ) )
        {
            snprintf( converted_number, sizeof( converted_number ), "%i ", key_index );
            strcat( string, converted_number );
        }
    }

    // Print the complete string to the terminal
    puts( string );
}


//**************************************************************************************************
// End of file
//**************************************************************************************************
