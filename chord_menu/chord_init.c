//**************************************************************************************************
// File:   chord_init.c
// Author: James Williamson
// Date:   9/21/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Initializes the Chord simulation by reading in a list of initial key values from an external
// data file.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chord_config.h"
#include "chord_debug.h"
#include "chord_init.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// File delimiters (commas are used to delimit keys)
static const char file_string_delimiters[] = ", \r\n\t";


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// The list of initial keys read from the data file
static int initial_key_list[MAX_KEY_VALUE];

// The number of keys in the list
static int number_of_keys = 0;


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
chord_err_t init_key_list()
{
    // Local variables
    FILE *file_handle;                      // A pointer to the data file that stores the keys
    chord_err_t err;                        // Return status code
    char file_buffer[MAX_FILE_SIZE_CHARS];  // Buffer for content of the key data file
    char *token;                            // Pointer to a key (in single string token format)
    int key;                                // Holds a key (converted integer format)
    int file_input;                         // Holds a character (or EOF value) read from the file
    int index;                              // Index for file input buffer
    
    // Initialization
    err = CHORD_ERR_NONE;
    index = 0;
    
    // Open the file
    file_handle = fopen( file_path, "r" );
    
    if( file_handle == NULL )
    {
        // Unable to open file
        err = CHORD_ERR_KEY_FILE;
        debug_printf( "[DBG] Error: Unable to open the key data file.\n" );
    }
    else
    {
        // File is opened; read the keys
        file_input = fgetc( file_handle );
        
        // Continue until end of file is found or max read capacity is reached
        while( ( file_input != EOF ) && ( index < MAX_FILE_SIZE_CHARS - 1 ) )
        {
            file_buffer[index++] = (char)file_input;
            file_input = fgetc( file_handle );
        }
        
        debug_printf( "[DBG] Info: Read %i characters from file.\n", index );
        
        // Once complete, make sure a null-terminator is added
        file_buffer[index] = '\0';
        
        // Next, tokenize to retrieve the individual keys, and store them
        token = strtok( file_buffer, file_string_delimiters );
        
        while( token != NULL )
        {
            // Convert token to integer
            key = atoi( token );
            
            // Check range - if it's a valid key, add it to the list
            if( ( key >= 0 ) && ( key < MAX_KEY_VALUE ) )
            {
                initial_key_list[number_of_keys++] = key;
            }
            
            // Continue tokenizing process
            token = strtok( NULL, file_string_delimiters );
        }
        
        // Debug: ensure the operation went right
        debug_printf( "[DBG] Info: Read %i keys from file.\n", number_of_keys );
        debug_printf( "[DBG] Info: Keys are: " );
        
        for( int index = 0; index < number_of_keys; index++ )
        {
            debug_printf( "%i, ", initial_key_list[index] );
        }
        
        debug_printf( "\n" );
        
        // Done, close file
        fclose( file_handle );
    }
    
    return( err );
}


/***************************************************************************************************
 * Function: init_get_key_count
 * 
 * Get the initial number of keys that the DHT should hold.
 * 
 * param:  void
 * return: The number of keys initially in the DHT (read from data file)
 **************************************************************************************************/
int init_get_key_count()
{
    return( number_of_keys );
}


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
int *init_get_key_list()
{
    return( &initial_key_list[0] );
}



//**************************************************************************************************
// End of file.
//**************************************************************************************************
