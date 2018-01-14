//**************************************************************************************************
// File:   chord_debug.c
// Author: James Williamson
// Date:   9/21/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Provides debug print capability for the Chord simulation. Debug prints may be turned on or off
// during program runtime.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include "chord_debug.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// Debug setting
static bool debug_on = false;


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: debug_enable_prints
 * 
 * Enables debug print output to the standard output stream.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void debug_enable_prints()
{
    debug_on = true;
}


/***************************************************************************************************
 * Function: debug_disable_prints
 * 
 * Disables debug print output to the standard output stream.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void debug_disable_prints()
{
    debug_on = false;
}


/***************************************************************************************************
 * Function: debug_printf
 * 
 * Write a line of debug output to the standard output stream. If prints are disabled, this 
 * function will simply return without taking any action.
 * 
 * param:  A formatted debug string to print
 * return: void
 **************************************************************************************************/
void debug_printf( const char *format, ... )
{
    // Only print if debug is enabled
    if( debug_on == true )
    {
        // Translate to printf call
        va_list args;
        va_start( args, format );
        vprintf( format, args );
        va_end( args );
    }
}


//**************************************************************************************************
// End of file.
//**************************************************************************************************
