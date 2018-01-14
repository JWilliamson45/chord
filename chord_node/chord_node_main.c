//**************************************************************************************************
// File:   chord_node_main.c
// Author: James Williamson
// Date:   9/22/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Application entrypoint for a Chord node program. This program is not intended to be executed
// as a standalone application (under typical usage); rather, it is expected that it is invoked
// by a (parent) menu process, which will pipe in commands.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "chord_debug.h"
#include "chord_node.h"


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
 * Function: main
 * 
 * Chord node application entrypoint.
 * 
 **************************************************************************************************/
int main(int argc, char** argv) 
{
    // Local variables
    int menu_pipe_handle;         // The file descriptor to receive data from the menu program
    
    // Disable debug prints by default
    debug_disable_prints();
    
    // Retrieve file descriptor so menu program can send commands
    sscanf( argv[0], "%i", &menu_pipe_handle );
    
    // Initialize "anchor" node
    init_dht( menu_pipe_handle );
    
    while( true )
    {
        // Continually check messages
        check_messages();
    }
    
    return( EXIT_SUCCESS );
}


//**************************************************************************************************
// End of file.
//**************************************************************************************************
