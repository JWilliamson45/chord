//**************************************************************************************************
// File:   chord_init.h
// Author: James Williamson
// Date:   9/21/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Initializes the Chord simulation by reading in a list of initial key values from an external
// data file.
//
// Note: An assumption made by this program is that the data file is named "key.dat" and is
//       present in the working directory. For optimal flexibility, this should later be updated
//       to be configurable; this is outside the scope of the assignment.
// 
//**************************************************************************************************

#ifndef CHORD_INIT_H
#define CHORD_INIT_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

#include "chord_error.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: init_key_list
 * 
 * Reads in the initial list of keys to be used in the DHT from an external data file. Once the
 * keys are read from the file, they are stored locally to be transferred to the initial node 
 * later.
 * 
 * Note that if a malformed or out-of-range key is present in the file, it is discarded and not 
 * added to the list.
 * 
 * param:  void
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t init_key_list();


/***************************************************************************************************
 * Function: init_get_key_count
 * 
 * Get the initial number of keys that the DHT should hold.
 * 
 * param:  void
 * return: The number of keys initially in the DHT (read from data file)
 **************************************************************************************************/
int init_get_key_count();


/***************************************************************************************************
 * Function: init_get_key_list
 * 
 * Returns a pointer to the list of initial keys read from the data file.
 * 
 * Note: This should be used in conjunction with init_get_key_count to avoid reading invalid data.
 * 
 * param:  void
 * return: A pointer to the list of keys read from the file
 **************************************************************************************************/
int *init_get_key_list();


#endif

//**************************************************************************************************
// End of file.
//**************************************************************************************************
