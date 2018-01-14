//**************************************************************************************************
// File:   chord_menu.c
// Author: James Williamson
// Date:   9/20/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Handles menu-based I/O via terminal for the user to interact with the DHT. This menu can be
// considered the "primary" process from which all others are forked.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chord_config.h"
#include "chord_commands.h"
#include "chord_menu.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// Maximum number of characters able to be read from a single command
#define MAX_KEYBOARD_INPUT_CHARS                16

// String menu
static const char menu[] =
    "Welcome to JW's Chord DHT simulation.\n"
    "Please enter one of the following commands:\n"
    "  \"addnode\" - Add a new node to the DHT\n"
    "  \"dump\"    - Display the content topology of the DHT\n"
    "  \"addkey\"  - Add a key to the DHT\n"
    "  \"delkey\"  - Delete a key from the DHT\n"
    "  \"menu\"    - Redisplay this menu on the terminal\n"
    "  \"debug\"   - Toggle debug messages (developer only)\n"
    "  \"exit\"    - Exit the program\n";

// Prompts for additional input
static const char prompt_addkey[] = 
    "Enter a key value to add to the DHT (must be between 0-63, inclusive).\n";

static const char prompt_delkey[] =
    "Enter a key value to delete from the DHT (must be between 0-63, inclusive).\n";

// Error strings
static const char input_error[] =
    "Invalid input. You must enter a value between 0-63, inclusive.\n";

static const char unrecognized_cmd[] = 
    "Command not recognized; please try again\n";

// Accepted commands
static const char menu_add_node[] = "addnode\n";
static const char menu_dump[] = "dump\n";
static const char menu_add_key[] = "addkey\n";
static const char menu_del_key[] = "delkey\n";
static const char menu_show_menu[] = "menu\n";
static const char menu_debug[] = "debug\n";
static const char menu_exit[] = "exit\n";

// Local prototypes
static void menu_process_addnode_cmd();
static void menu_process_addkey_cmd();
static void menu_process_delkey_cmd();


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// Holds user input
static char user_input[MAX_KEYBOARD_INPUT_CHARS];


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: menu_execute
 * 
 * Run the menu for user I/O. The system is command-driven; this function will not return until
 * the system receives an "exit" command.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void menu_execute()
{
    // Local variables
    bool execute = true;         // Flag: "keep executing the program"
    
    // Display menu once before prompting user
    fputs( menu, stdout );
    
    while( execute )
    {
        // Get user input
        if( fgets( user_input, MAX_KEYBOARD_INPUT_CHARS, stdin ) != NULL )
        {
            // Process command or output error
            if( strcmp( user_input, menu_add_node ) == 0 )
            {
                menu_process_addnode_cmd();
            }
            else if( strcmp( user_input, menu_dump ) == 0 )
            {
                cmd_dump();
            }
            else if( strcmp( user_input, menu_add_key ) == 0 )
            {
                menu_process_addkey_cmd();
            }
            else if( strcmp( user_input, menu_del_key ) == 0 )
            {
                menu_process_delkey_cmd();
            }
            else if( strcmp( user_input, menu_show_menu ) == 0 )
            {
                // Redisplay the menu for the user
                fputs( menu, stdout );
            }
            else if( strcmp( user_input, menu_debug ) == 0 )
            {
                cmd_toggle_debug();
            }
            else if( strcmp( user_input, menu_exit ) == 0 )
            {
                // Kill all processes in this group
                kill( 0, SIGKILL );
                
                // Force return from this function to terminate the program
                execute = false;
            }
            else
            {
                // The user entered an invalid command
                fputs( unrecognized_cmd, stdout );
            }
        }
    }
}


/***************************************************************************************************
 * Function: menu_process_addnode_cmd
 * 
 * Helper function that processes the "addnode" cmd from the user.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
static void menu_process_addnode_cmd()
{
    // Local variables
    chord_err_t err;            // An error code that may be returned by the command
    
    // Attempt to add a new node
    err = cmd_add_node();
    
    if( err == CHORD_ERR_MAX_NODES )
    {
        fputs( "Unable to add node: the DHT has reached the maximum number of nodes\n", stdout );
    }
    else
    {
        fputs( "New node added!\n", stdout );
    }
}


/***************************************************************************************************
 * Function: menu_process_addkey_cmd
 * 
 * Helper function that processes the "addkey" cmd from the user.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
static void menu_process_addkey_cmd()
{
    // Local variables
    int parsed_id = 0;           // Holds a parsed ID from the user (if applicable)
    chord_err_t err;             // An error code that may be returned by the command
    
    // Prompt user for the key number to add
    fputs( prompt_addkey, stdout );

    // Get entered value
    if( fgets( user_input, MAX_KEYBOARD_INPUT_CHARS, stdin ) != NULL )
    {
        // Try to convert to an integer and report an error if the operation fails
        errno = 0;
        parsed_id = strtol( user_input, NULL, 10 );

        if( ( errno != 0 ) || ( parsed_id < 0 ) || ( parsed_id >= MAX_NODE_COUNT ) )
        {
            // Tell user input is invalid
            fputs( input_error, stdout );
        }
        else
        {
            // ID is good - attempt to add the key
            err = cmd_add_key( parsed_id );
            
            if( err == CHORD_ERR_KEY_ALREADY_ADDED )
            {
                printf( "Unable to add key: <%i> is already in the DHT\n", parsed_id );
            }
            else if( err == CHORD_ERR_INVALID_KEY )
            {
                printf( "Unable to add key: <%i> is an invalid key value\n", parsed_id );
            }
            else
            {
                printf( "New key <%i> added!\n", parsed_id );
            }
        }
    }
}


/***************************************************************************************************
 * Function: menu_process_delkey_cmd
 * 
 * Helper function that processes the "delkey" cmd from the user.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
static void menu_process_delkey_cmd()
{
    // Local variables
    int parsed_id = 0;           // Holds a parsed ID from the user (if applicable)
    chord_err_t err;             // An error code that may be returned by the command
    
    // Prompt user for the key number to add
    fputs( prompt_delkey, stdout );

    // Get entered value
    if( fgets( user_input, MAX_KEYBOARD_INPUT_CHARS, stdin ) != NULL )
    {
        // Try to convert to an integer and report an error if the operation fails
        errno = 0;
        parsed_id = strtol( user_input, NULL, 10 );

        if( ( errno != 0 ) || ( parsed_id < 0 ) || ( parsed_id >= MAX_NODE_COUNT ) )
        {
            // Tell user input is invalid
            fputs( input_error, stdout );
        }
        else
        {
            // ID is good - attempt to delete the key
            err = cmd_delete_key( parsed_id );
            
            if( err == CHORD_ERR_NO_SUCH_KEY )
            {
                printf( "Unable to delete key: <%i> is not in the DHT\n", parsed_id );
            }
            else if( err == CHORD_ERR_INVALID_KEY )
            {
                printf( "Unable to delete key: <%i> is an invalid key value\n", parsed_id );
            }
            else
            {
                printf( "Key <%i> deleted!\n", parsed_id );
            }
        }
    }
}


//**************************************************************************************************
// End of file.
//**************************************************************************************************
