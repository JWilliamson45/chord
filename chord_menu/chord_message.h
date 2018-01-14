//**************************************************************************************************
// File:   chord_message.h
// Author: James Williamson
// Date:   9/25/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Definition of Chord IPC messages.
// 
//**************************************************************************************************

#ifndef CHORD_MESSAGE_H
#define	CHORD_MESSAGE_H


//**************************************************************************************************
// Includes
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// Definition for the ID of the menu process, which is not technically a node
#define MENU_PROCESS_ID    64        // Outside of normal node range 0-63
#define MAIN_DHT_NODE      63        // The main node (communicates with the menu process)

// Chord command types
typedef enum
{
    RESERVED               = 1,      // Reserved command (zero is unused)
    ADD_NODE               = 2,      // Add a new node to the DHT
    ADD_KEY                = 3,      // Add a key to the DHT
    DELETE_KEY             = 4,      // Delete a key from the DHT
    DUMP                   = 5,      // Display the content topology of the DHT
    ANNOUNCE               = 6,      // Announce insertion of a new node (initiates key redist.)
    REDIST_KEY             = 7,      // Redistribute keys properly among the (updated) DHT ring
    TOGGLE_DEBUG           = 8,      // Turn on/off debug prints
} chord_cmd_t;

// A message that can be transmitted between nodes/processes
typedef struct
{
    chord_cmd_t cmd;                 // A command to process
    int id;                          // If applicable, a node ID or key ID
    int sender;                      // The node that is sending the message
} chord_msg_t;


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
