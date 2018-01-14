//**************************************************************************************************
// File:   chord_commands.c
// Author: James Williamson
// Date:   9/25/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Contains routines to build and send Chord commands to the DHT node ring.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "chord_config.h"
#include "chord_commands.h"
#include "chord_debug.h"
#include "chord_error.h"
#include "chord_init.h"
#include "chord_message.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// Size of the buffer used to pass arguments to new node program
static const int arg_buffer_size = 32;

// The communication pipe used to send commands to the DHT main node
static int pipe_to_main_node[2];

// Tracks created nodes (duplicate IDs are not allowed)
static uint64_t created_nodes = 0;

// Tracks keys in the DHT (duplicate keys are not allowed)
static uint64_t dht_keys = 0;

// Local prototypes
static void cmd_populate_main_node();


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// Tracks whether debug messages are enabled or not
static bool debug_mode = false;


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
chord_err_t cmd_create_main_node()
{
    // Local variables
    pid_t process_id;                       // Holds a process ID for the fork operation
    chord_err_t err;                        // Return status code
    int exec_code;                          // Return value from new process exec call
    int errno_val;                          // Stores errno after a system call failure
    char arg[arg_buffer_size];              // Holds an argument to pass to the child program
    
    // Initialization
    err = CHORD_ERR_NONE;
    
    // Create pipe
    if( pipe( pipe_to_main_node ) == 0 )
    {
        // Success; now create child process
        process_id = fork();

        if( process_id == 0 )
        {
            /**
             * Have the child execute a new program; need to send it the pipe "read" handle as a 
             * string, so that it can receive commands from the menu process.
             * 
             * TODO: change this to current working directory
             */
            sprintf( arg, "%i", pipe_to_main_node[0] );
            
            exec_code = execl( "//home//jwilliamson//NetBeansProjects//chord_node//dist//Debug//GNU-Linux//chord_node",
                               arg, (char *)NULL );

            if( exec_code == -1 )
            {
                // Something went wrong; store errno value
                errno_val = errno;

                // Inform user and terminate this child process 
                debug_printf( "[DBG] Error: unable to start initial node process (errno: %i)\n", 
                              errno_val );
                
                err = CHORD_ERR_INIT;
                exit( EXIT_FAILURE );
            }
        }
        else if( process_id == -1 )
        {
            // Something went wrong; store errno value
            errno_val = errno;

            // Inform user
            debug_printf( "[DBG] Error: creation of initial node process failed (errno: %i)\n", 
                          errno_val );
            
            err = CHORD_ERR_INIT;
        }
        else
        {
            // Success - mark initial node (ID is always 63) as created
            created_nodes |= ( 1UL << MAIN_DHT_NODE );
            
            // Populate main node with keys read from the data file
            cmd_populate_main_node();
        }
    }
    else
    {
        // Something went wrong; store errno value
        errno_val = errno;
        
        // Inform user
        debug_printf( "[DBG] Error: creation of main node pipe failed (errno: %i)\n", 
                      errno_val );
        
        err = CHORD_ERR_INIT;
    }
    
    return( err );
}


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
chord_err_t cmd_add_node()
{
    // Local variables
    chord_msg_t msg;          // A message to pass to the main node
    chord_err_t err;          // An error code to return from the function
    int new_node_id;          // The (randomly-generated) ID for the new node
    bool found_id;            // Flag: "found a suitable ID"
    
    // Initialization
    err = CHORD_ERR_NONE;
    found_id = false;
    
    // If all nodes are created, do nothing and return error code
    if( created_nodes == UINT64_MAX )
    {
        err = CHORD_ERR_MAX_NODES;
    }
    else
    {
        /**
         * Generate a new ID for the node to add using a random number source. Keep going until
         * an unused number is obtained.
         */
        while( found_id == false )
        {
            // Seed random number generator with the current time
            srand( time( NULL ) );
            new_node_id = ( rand() % ( MAX_NODE_COUNT - 1 ) );
            
            if( ( created_nodes & ( 1UL << new_node_id ) ) == 0 )
            {
                // Found a suitable ID
                found_id = true;
                created_nodes |= ( 1UL << new_node_id );
            }
        }
        
        // Build the message
        msg.cmd = ADD_NODE;
        msg.id = new_node_id;
        msg.sender = MENU_PROCESS_ID;
        
        // Debug
        debug_printf( "[DBG] Info: Command <addnode> adding new node ID %i into DHT ring\n", 
                       new_node_id );
        
        // Send to main node
        write( pipe_to_main_node[1], (void *)&msg, sizeof( msg ) );
    }
    
    return( err );
}


/***************************************************************************************************
 * Function: cmd_add_key
 * 
 * Command to add a new key to the DHT ring. If the key is already present in the DHT, an error
 * is returned; it will not be added again.
 * 
 * param:  the key to be added to the DHT
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t cmd_add_key( int key_id )
{
    // Local variables
    chord_msg_t msg;          // A message to pass to the main node
    chord_err_t err;          // An error code to return from the function
    
    // Initialization
    err = CHORD_ERR_NONE;
    
    // Check to ensure key is valid
    if( ( key_id < 0 ) || ( key_id >= MAX_KEY_VALUE ) )
    {
        err = CHORD_ERR_INVALID_KEY;
    }
    else
    {
        if( dht_keys & ( 1UL << key_id ) )
        {
            // Key is already in the DHT
            err = CHORD_ERR_KEY_ALREADY_ADDED;
        }
        else
        {
            // Build the message
            msg.cmd = ADD_KEY;
            msg.id = key_id;
            msg.sender = MENU_PROCESS_ID;
        
            // Debug
            debug_printf( "[DBG] Info: Command <addkey> adding new key ID %i into DHT ring\n", 
                          key_id );
            
            // Mark key as active
            dht_keys |= ( 1UL << key_id );
        
            // Send to main node
            write( pipe_to_main_node[1], (void *)&msg, sizeof( msg ) );
        }
    }
    
    return( err );
}


/***************************************************************************************************
 * Function: cmd_delete_key
 * 
 * Command to delete a key from the DHT ring. If the key is not present in the DHT, an error
 * is returned.
 * 
 * param:  the key to be removed from the DHT
 * return: An error code indicative of success or failure
 **************************************************************************************************/
chord_err_t cmd_delete_key( int key_id )
{
    // Local variables
    chord_msg_t msg;          // A message to pass to the main node
    chord_err_t err;          // An error code to return from the function
    
    // Initialization
    err = CHORD_ERR_NONE;
    
    // Check to ensure key is valid
    if( ( key_id < 0 ) || ( key_id >= MAX_KEY_VALUE ) )
    {
        err = CHORD_ERR_INVALID_KEY;
    }
    else
    {
        if( ( dht_keys & ( 1UL << key_id ) ) == 0 )
        {
            // Can't remove; key is not in DHT
            err = CHORD_ERR_NO_SUCH_KEY;
        }
        else
        {
            // Build the message
            msg.cmd = DELETE_KEY;
            msg.id = key_id;
            msg.sender = MENU_PROCESS_ID;
        
            // Debug
            debug_printf( "[DBG] Info: Command <delkey> removing key ID %i from DHT ring\n", 
                          key_id );
            
            // Mark key as inactive
            dht_keys &= ~( 1UL << key_id );
        
            // Send to main node
            write( pipe_to_main_node[1], (void *)&msg, sizeof( msg ) );
        }
    }
    
    return( err );
}


/***************************************************************************************************
 * Function: cmd_dump
 * 
 * Command to have all nodes in the DHT dump their ID and key set to standard output.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void cmd_dump()
{
    // Local variables
    chord_msg_t msg;          // A message to pass to the main node
    
    // Build the message
    msg.cmd = DUMP;
    msg.id = 0;
    msg.sender = MENU_PROCESS_ID;

    // Debug
    debug_printf( "[DBG] Info: Command <dump> sent to DHT\n" );

    // Send to main node
    write( pipe_to_main_node[1], (void *)&msg, sizeof( msg ) );
}


/***************************************************************************************************
 * Function: cmd_toggle_debug
 * 
 * Command to toggle the output of debug messages. This is useful for development purposes. By 
 * default, debug is disabled.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void cmd_toggle_debug()
{
    // Local variables
    chord_msg_t msg;          // A message to pass to the main node
    
    // Build the message
    msg.cmd = TOGGLE_DEBUG;
    msg.sender = MENU_PROCESS_ID;
    
    if( debug_mode == true )
    {
        // Turn off debug
        debug_mode = false;
        debug_disable_prints();
        msg.id = 0;
        printf( "Debug messages disabled.\n" );
    }
    else
    {
        // Turn on debug
        debug_mode = true;
        debug_enable_prints();
        msg.id = 1;
        printf( "Debug messages enabled.\n" );
    }
    
    // Send to main node
    write( pipe_to_main_node[1], (void *)&msg, sizeof( msg ) );
}


/***************************************************************************************************
 * Function: cmd_populate_main_node
 * 
 * Send the initial key list to the main node. This uses a sequence of "add key" messages through
 * a pipe so that DHT initialization can be completed.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
static void cmd_populate_main_node()
{
    // Local variables
    chord_msg_t msg;          // A message to pass to the main node
    int number_of_keys;       // The number of initial keys
    int *key;                 // Pointer to a key (points to list of keys)
    
    // Initialization
    msg.cmd = ADD_KEY;
    msg.sender = MENU_PROCESS_ID;
    number_of_keys = init_get_key_count();
    key = init_get_key_list();
    
    if( number_of_keys > 0 )
    {
        // Send an "add key" command for each key in the list
        while( number_of_keys > 0 )
        {
            // Get next key ID and place into message
            msg.id = *key;
            
            // Mark key as active
            dht_keys |= ( 1UL << msg.id );
            
            // Send to main node
            write( pipe_to_main_node[1], (void *)&msg, sizeof( msg ) );
            
            key++;
            number_of_keys--;
        }
    }
}


//**************************************************************************************************
// End of file.
//**************************************************************************************************
