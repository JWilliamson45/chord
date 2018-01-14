//**************************************************************************************************
// File:   chord_menu_main.c
// Author: James Williamson
// Date:   9/20/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// This is the main entrypoint for the Chord simulation program. This application uses fundamental
// Unix calls to carry out a limited version of the Chord distributed hash table algorithm.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stdlib.h>
#include "chord_commands.h"
#include "chord_debug.h"
#include "chord_init.h"
#include "chord_menu.h"


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: main
 * 
 * Main application entrypoint.
 * 
 **************************************************************************************************/
int main(int argc, char** argv) 
{
    // Disable debug prints by default
    debug_disable_prints();
    
    // Read initial list of keys from data file
    init_key_list();
    
    // Create the main (initial) DHT node
    cmd_create_main_node();
    
    // Run the menu that handles user I/O
    menu_execute();
    
    // Terminate
    return( EXIT_SUCCESS );
}


//**************************************************************************************************
// End of file.
//**************************************************************************************************
