//**************************************************************************************************
// File:   chord_config.h
// Author: James Williamson
// Date:   9/23/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Configuration constants for the Chord simulation program.
// 
//**************************************************************************************************

#ifndef CHORD_CONFIG_H
#define	CHORD_CONFIG_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// Maximum supported number of nodes
// Note: Increasing beyond 64 will break compatibility with the program
#define MAX_NODE_COUNT              64

// Maximum supported range of keys 
// Note: Increasing beyond 64 will break compatibility with the chord_key_set data structure
#define MAX_KEY_VALUE               64

// The maximum allowable characters that can be read from the key data file
#define MAX_FILE_SIZE_CHARS         512

// The key data file path
static const char file_path[] = ".//key.dat";


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module functions
//**************************************************************************************************

// (none)


#endif

//**************************************************************************************************
// End of file
//**************************************************************************************************
