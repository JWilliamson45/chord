//**************************************************************************************************
// File:   chord_error.h
// Author: James Williamson
// Date:   9/21/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Contains error codes and definitions for the Chord simulation program.
// 
//**************************************************************************************************

#ifndef CHORD_ERROR_H
#define CHORD_ERROR_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

typedef enum
{
    CHORD_ERR_NONE                = 0,      // No error
    CHORD_ERR_KEY_FILE            = 1,      // Keys could not be read from the data file
    CHORD_ERR_INIT                = 2,      // Unable to initialize the Chord DHT
    CHORD_ERR_MAX_NODES           = 3,      // Maximum supported number of nodes already created
    CHORD_ERR_INVALID_KEY         = 4,      // Invalid key value was entered by user
    CHORD_ERR_KEY_ALREADY_ADDED   = 5,      // The key is already in the DHT
    CHORD_ERR_NO_SUCH_KEY         = 6,      // The key is not found in the DHT
} chord_err_t;


//**************************************************************************************************
// Module functions
//**************************************************************************************************

// (none)


#endif

//**************************************************************************************************
// End of file.
//**************************************************************************************************
