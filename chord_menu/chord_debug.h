//**************************************************************************************************
// File:   chord_debug.h
// Author: James Williamson
// Date:   9/21/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Provides debug print capability for the Chord simulation. Debug prints may be turned on or off
// during program runtime.
// 
//**************************************************************************************************

#ifndef CHORD_DEBUG_H
#define CHORD_DEBUG_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// (none)


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
void debug_enable_prints();


/***************************************************************************************************
 * Function: debug_disable_prints
 * 
 * Disables debug print output to the standard output stream.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void debug_disable_prints();


/***************************************************************************************************
 * Function: debug_printf
 * 
 * Write a line of debug output to the standard output stream. If prints are disabled, this 
 * function will simply return without taking any action.
 * 
 * param:  A formatted debug string to print
 * return: void
 **************************************************************************************************/
void debug_printf( const char *format, ... );


#endif

//**************************************************************************************************
// End of file.
//**************************************************************************************************