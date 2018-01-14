//**************************************************************************************************
// File:   chord_node.c
// Author: James Williamson
// Date:   9/26/2016
// 
// CIS620 Assignment 1 - Fall 2016
// 
// Models a Chord node process that is part of the DHT. The program is capable of forking itself
// to create new (local-only) nodes if a command to add a node is received from the menu process.
// A Chord node handles the addition or removal of keys from its local set, the scheme of which 
// is based on its ID and place in the DHT ring.
//
// Each node knows its successor information and local keys, but not the complete DHT node list or
// total keys present in the system. The intent is to decentralize the algorithm.
// 
//**************************************************************************************************

//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "chord_debug.h"
#include "chord_node.h"
#include "chord_config.h"
#include "chord_key_set.h"


//**************************************************************************************************
// Module definitions
//**************************************************************************************************

// The identification number of the node
static int node_id;

// The identification number of the node's successor
static int successor_id;

// Used to track whether this node has a successor
static bool has_successor;

// The pipe descriptor for receiving commands from the menu
static int pipe_from_menu;

// Pipe descriptors for communication between DHT nodes
static int dht_pipes[MAX_NODE_COUNT][2];

// Local prototypes
static void process_msg( chord_msg_t rx_msg );
static void process_add_node( chord_msg_t msg );
static void process_node_announcement( chord_msg_t msg );
static void process_add_key( chord_msg_t msg );
static void process_redist_key( chord_msg_t msg );
static void process_delete_key( chord_msg_t msg );
static void process_dump( chord_msg_t msg );
static void process_toggle_debug( chord_msg_t msg );


//**************************************************************************************************
// Module variables
//**************************************************************************************************

// (none)


//**************************************************************************************************
// Module functions
//**************************************************************************************************

/***************************************************************************************************
 * Function: init_dht
 * 
 * Initialize the main node of the distributed hash table, setting up the node attributes and
 * communication mechanisms.
 * 
 * param:  The pipe handle from the menu process, so that commands may be received
 * return: void
 **************************************************************************************************/
void init_dht( int menu_pipe_handle )
{
    // Setup "main node"
    node_id = MAIN_DHT_NODE;
    successor_id = INT_MAX;
    has_successor = false;
    
    // Ensure the node's key set is cleared
    keyset_init();
    
    // Assign menu pipe handle for receiving commands 
    pipe_from_menu = menu_pipe_handle;
    
    // Set up all DHT pipes
    for( int index = 0; index < MAX_NODE_COUNT; index++ )
    {
        pipe( dht_pipes[index] );
    }
    
    // Set only main node's pipes to nonblocking mode so it can look at multiple inputs
    fcntl( dht_pipes[MAIN_DHT_NODE][0], F_SETFL, O_NONBLOCK );
    fcntl( pipe_from_menu, F_SETFL, O_NONBLOCK );
}


/***************************************************************************************************
 * Function: check_messages
 * 
 * Check for incoming messages from other nodes in the DHT.
 * 
 * param:  void
 * return: void
 **************************************************************************************************/
void check_messages( void )
{
    // Local variables
    chord_msg_t rx_msg;         // Holds a received message
    ssize_t bytes_read;         // The number of bytes read
    
    // If this is the main node, check for messages from menu process
    if( node_id == MAIN_DHT_NODE )
    {
        bytes_read = read( pipe_from_menu, (void *)&rx_msg, sizeof( rx_msg ) );
        
        // Process command
        if( bytes_read == sizeof( rx_msg ) )
        {
            process_msg( rx_msg );
        }
    }
    
    // Wait for DHT messages
    bytes_read = read( dht_pipes[node_id][0], (void *)&rx_msg, sizeof( rx_msg ) );

    // Process command
    if( bytes_read == sizeof( rx_msg ) )
    {
        process_msg( rx_msg );
    }
}


/***************************************************************************************************
 * Function: process_msg
 * 
 * Process the given message, using its command and ID information to perform a specific action.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_msg( chord_msg_t rx_msg )
{
    switch( rx_msg.cmd )
    {
        case( ADD_NODE ):
            process_add_node( rx_msg );
            break;

        case( ADD_KEY ):
            process_add_key( rx_msg );
            break;

        case( DELETE_KEY ):
            process_delete_key( rx_msg );
            break;

        case( DUMP ):
            process_dump( rx_msg );
            break;
            
        case( ANNOUNCE ):
            process_node_announcement( rx_msg );
            break;

        case( REDIST_KEY ):
            process_redist_key( rx_msg );
            break;
            
        case( TOGGLE_DEBUG ):
            process_toggle_debug( rx_msg );
            break;
    }
}


/***************************************************************************************************
 * Function: process_add_node
 * 
 * Process the "addnode" command, which seeks to insert a node (in the proper sequence) into the
 * DHT. If the message ID is less than that of the successor, the current node forks and creates
 * the new node. Successor information is then updated appropriately to maintain the ring. If the 
 * message ID is not less than that of the successor, the message is forwarded and no action is
 * taken.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_add_node( chord_msg_t msg )
{
    // Local variables
    pid_t process_id;                     // Holds a process ID for the fork operation
    int errno_val;                        // Stores errno after a system call failure
    chord_msg_t announcement_msg;         // A message to announce new node insertion to successor
    
    if( msg.id < successor_id )
    {
        /*
         * If the new node ID is less than the successor ID, the node should be inserted
         * "in between" the two, so fork here.
         */
        process_id = fork();
        
        switch( process_id )
        {
            case( -1 ):
                
                // Something went wrong; store errno value
                errno_val = errno;

                // Inform user
                debug_printf( "[DBG] Error: creation of new node failed (id: %i, errno: %i)\n", 
                              msg.id, errno_val );
                
                break;
                
            case( 0 ):
                
                // Overwrite old node ID with new ID of the child process/node
                node_id = msg.id;

                /**
                 * Special case when adding the first (non-main) node: if successor ID is INT_MAX, 
                 * update it to 63, so that the two nodes point to each other as successors.
                 * Otherwise, for subsequent additions, the child will already have the correct
                 * successor ID - it's the parent that needs to update their copy.
                 */
                if( successor_id == INT_MAX )
                {
                    successor_id = MAIN_DHT_NODE;
                }
                
                // Initialize key set of new node
                keyset_init();

                debug_printf( "[DBG] Addnode: Node %i (PID: %i, PPID: %i, SUCC: %i) was created\n", 
                              node_id, getpid(), getppid(), successor_id );
                
                break;
                
            default:
                
                /*
                 * Before updating the successor ID to point to the new inserted node, send
                 * an announcement to original successor to initiate a key redistribution.
                 */
                announcement_msg.cmd = ANNOUNCE;
                announcement_msg.id = msg.id;
                announcement_msg.sender = node_id;
            
                /*
                 * Note: if there is no successor, (only main node exists), just have the message
                 * sent to self to process as any other node would.
                 */
                if( successor_id == INT_MAX )
                {
                    write( dht_pipes[node_id][1], (void *)&announcement_msg, 
                           sizeof( announcement_msg ) );
                }
                else
                {
                    write( dht_pipes[successor_id][1], (void *)&announcement_msg, 
                           sizeof( announcement_msg ) );
                }
                
                // Now, parent updates their successor ID to point to inserted node
                successor_id = msg.id;
                
                debug_printf( "[DBG] Addnode: Node %i (PID: %i, PPID: %i, SUCC: %i) spawned "
                              "new node, updated successor\n", node_id, getpid(), getppid(), 
                              successor_id );
                
                break;
        }
    }
    else
    {
        /*
         * If the new node ID is not less than the successor ID, forward the message to the next
         * node.
         */
        debug_printf( "[DBG] Info: Node %i is forwarding addnode<%i> to successor node %i\n",
                      node_id, msg.id, successor_id );
        
        write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
    }
}


/***************************************************************************************************
 * Function: process_node_announcement
 * 
 * Process a message that announces the insertion of a new node into the DHT ring. This is used 
 * to initiate key redistribution.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_node_announcement( chord_msg_t msg )
{
    // Local variables
    int key_index = 0;            // Used to index the local key set
    chord_msg_t redist_msg;       // A message indicating key re-distribution is occurring
    
    /*
     * If an announcement message is received by a node, that means the predecessor sent it to
     * signal that a new node was inserted between the predecessor and the current node. So, 
     * the current node must redistribute any keys that are less than the new node ID.
     */
    debug_printf( "[DBG] Info: Node %i received announcement of creation of node %i - "
                  "redistributing keys now\n", node_id, msg.id );
    
    for( key_index; key_index <= msg.id; key_index++ )
    {
        if( keyset_check( key_index ) == true )
        {
            // Delete the key from the local set and send it along
            keyset_remove( key_index );
            
            redist_msg.cmd = REDIST_KEY;
            redist_msg.id = key_index;
            redist_msg.sender = node_id;
            
            write( dht_pipes[successor_id][1], (void *)&redist_msg, sizeof( redist_msg ) );
        }
    }
}


/***************************************************************************************************
 * Function: process_add_key
 * 
 * Processes the "addkey" command, used to add a key to the DHT. If the key is not a correct match
 * for the node, based on the ID, it is forwarded appropriately.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_add_key( chord_msg_t msg )
{
    if( node_id == MAIN_DHT_NODE )
    {
        /*
         * The main node (63) will forward this message initially, unless it is the only node. 
         * Under typical operation (a ring is established), the key in question will only be 
         * added to the main node if it makes its way around the ring, not settling with an 
         * intermediate node.
         */
        if( msg.sender == MENU_PROCESS_ID )
        {
            if( successor_id == INT_MAX )
            {
                // Special case: there is no ring yet - so just add the key here.
                keyset_add( msg.id );
                
                debug_printf( "[DBG] Info: Node %i added key %i\n", node_id, msg.id );
            }
            else
            {
                // Received original command - change sender and forward to the rest of the ring
                msg.sender = MAIN_DHT_NODE;
                write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
            }
        }
        else if( msg.sender == MAIN_DHT_NODE )
        {
            // If the message got all the way around the ring, key should be placed here
            keyset_add( msg.id );
            
            debug_printf( "[DBG] Info: Node %i added key %i\n", node_id, msg.id );
        }
    }
    else
    {
        /*
         * Other nodes look at the message payload and compare it to their ID - if the key to 
         * add is larger than the node ID, forward the message. Else, add it to the local keyset.
         */
        if( msg.id > node_id )
        {
            write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
        }
        else
        {
            keyset_add( msg.id );
            
            debug_printf( "[DBG] Info: Node %i added key %i\n", node_id, msg.id );
        }
    }
}


/***************************************************************************************************
 * Function: process_redist_key
 * 
 * Processes a message that is used to redistribute keys across the DHT. If the key is not a 
 * correct match for the node, based on the ID, it is forwarded appropriately.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_redist_key( chord_msg_t msg )
{
    /*
     * Process a key redistribution message. If the current node ID is greater than that of the
     * sender, the key needs to keep moving through the ring to find the added node.
     */
    if( node_id > msg.sender )
    {
        write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
    }
    else
    {
        // Current node ID is less than the sender, so look at the key
        if( msg.id > node_id )
        {
            // Key is larger than the current node id, so forward the key
            write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
        }
        else
        {
            // Key is not larger (or equal to) node id - redistribution complete
            keyset_add( msg.id );
            
            debug_printf( "[DBG] Info: Node %i added redistributed key %i\n", node_id, msg.id );
        }
    }
}


/***************************************************************************************************
 * Function: process_delete_key
 * 
 * Processes the "delkey" command, which is used to remove a key from the DHT. If the key is not 
 * in the local key set of the node, it is forwarded appropriately.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_delete_key( chord_msg_t msg )
{
    if( node_id == MAIN_DHT_NODE )
    {
        /*
         * The main node (63) will forward this message initially, unless it is the only node. If
         * it is the only node, or the message has made its way around the ring back to it, the
         * key in question should be a part of the local key set, by process of elimination (the
         * menu will bounce the request if the key was not tracked as "added").
         */
        if( msg.sender == MENU_PROCESS_ID )
        {
            if( successor_id == INT_MAX )
            {
                // Special case: there is no ring yet - remove the key from the local set
                keyset_remove( msg.id );
                
                debug_printf( "[DBG] Info: Node %i removed key %i\n", node_id, msg.id );
            }
            else
            {
                // Received original command - change sender and forward to the rest of the ring
                msg.sender = MAIN_DHT_NODE;
                write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
            }
        }
        else if( msg.sender == MAIN_DHT_NODE )
        {
            // If the message got all the way around the ring, key should be here, so remove it
            keyset_remove( msg.id );
            
            debug_printf( "[DBG] Info: Node %i removed key %i\n", node_id, msg.id );
        }
    }
    else
    {
        // If the key is here, remove it. Otherwise, send the message along.
        if( keyset_check( msg.id ) )
        {
            keyset_remove( msg.id );
            
            debug_printf( "[DBG] Info: Node %i removed key %i\n", node_id, msg.id );
        }
        else
        {
            write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
        }
    }
}


/***************************************************************************************************
 * Function: process_dump
 * 
 * Process the "dump" command, which instructs each node to print their current key set and ID to
 * the console. This message is forwarded to all nodes in the ring.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_dump( chord_msg_t msg )
{
    if( node_id == MAIN_DHT_NODE )
    {
        /*
         * The main node (63) will receive this message twice; once from the menu process to
         * initiate the DHT dump, and again when the message has been forwarded around the
         * ring. Upon second receipt is when 63 dumps its key set (to maintain proper ascending 
         * order on the console printout). Then, the message is not forwarded again.
         */
        if( msg.sender == MENU_PROCESS_ID )
        {
            if( successor_id == INT_MAX )
            {
                // Special case: there is no ring yet - only this node. So just dump.
                printf( "Node %i owns keys: ", node_id );
                keyset_print();
            }
            else
            {
                // Received original command - change sender and forward to the rest of the ring
                msg.sender = MAIN_DHT_NODE;
                write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
            }
        }
        else if( msg.sender == MAIN_DHT_NODE )
        {
            // Now, dump main node's key set and don't forward again
            printf( "Node %i owns keys: ", node_id );
            keyset_print();
        }
    }
    else
    {
        // Dump to console
        printf( "Node %i owns keys: ", node_id );
        keyset_print();
        
        // Forward to next node
        write( dht_pipes[successor_id][1], (void *)&msg, sizeof( msg ) );
    }
}


/***************************************************************************************************
 * Function: process_toggle_debug
 * 
 * Toggles the output of debug messages. This is useful for development purposes. By default,
 * debug is disabled.
 * 
 * param:  A message received from another process/node
 * return: void
 **************************************************************************************************/
static void process_toggle_debug( chord_msg_t msg )
{
    if( msg.id == 0 )
    {
        // Turn off debug
        debug_disable_prints();
    }
    else
    {
        // Turn on debug
        debug_enable_prints();
    }
}


//**************************************************************************************************
// End of file.
//**************************************************************************************************
