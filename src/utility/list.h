#ifndef OS_LIST_H
#define OS_LIST_H

#include "types.h"

#define OFFSET_OF( TYPE, MEMBER ) ((size_t)&(((TYPE*)0)->MEMBER))
#define CONTAINER_OF( TYPE, MEMBER, NODE ) (TYPE*)(((uint8_t*)(NODE)) - OFFSET_OF( TYPE, MEMBER ))

typedef struct list_node_t {
    struct list_node_t* next;
    struct list_node_t* prev;
} list_node_t;

typedef struct {
    list_node_t head;
} list_head_t;

/**
 * Initialise a circular list object
 * @param list The list object to initialise
 */
void init_list( list_head_t* list );

/**
 * Check if a list is empty
 * @param list The list to check
 * @return True if it is empty, false otherwise
 */
bool list_is_empty( list_head_t* list );

/**
 * Get the next node in the list
 * @param curr The node whose successor to get
 * @return The nodes successor
 */
list_node_t* list_get_next_node( list_node_t* curr );

/**
 * Get the previous node in the list
 * @param curr The node whose predecessor to get
 * @return The node's predecessor
 */
list_node_t* list_get_prev_node( list_node_t* curr );

/**
 * Insert a node into the list after an existing node
 * @param node The node in the list to insert after
 * @param to_insert The node to insert
 */
void list_insert_after_node( list_node_t* node, list_node_t* to_insert );

/**
 * Insert a node into the list before an existing node
 * @param node The node in the list to insert before
 * @param to_insert The node to insert
 */
void list_insert_before_node( list_node_t* node, list_node_t* to_insert );

/**
 * Insert a node at the tail of the provided list
 * @param list The list to insert into
 * @param node The node to add to the list
 */
void list_insert_tail_node( list_head_t* list, list_node_t* node );

/**
 * Insert a node at the head of the the provide list
 * @param list The list to insert into
 * @param node The node to add to the list
 */
void list_insert_head_node( list_head_t* list, list_node_t* node );

/**
 * Remove a node from its containing list
 * @param node The node to remove
 */
void list_remove_node( list_node_t* node );

/**
 * Pop the head node from the provided list
 * @param list The list to pop from
 * @return The popped head node of the list
 */
list_node_t* list_pop_head( list_head_t* list );

/**
 * Pop the tail node from the provided list
 * @param list The list to pop from
 * @return The popped tail node of the list
 */
list_node_t* list_pop_tail( list_head_t* list );

/**
 * Replace the provided node with the new node at the same place
 * @param old The node to replace
 * @param new The new node to insert
 */
void list_replace_node( list_node_t* old, list_node_t* new );

/**
 * Advance the head of the list by one
 * @param list The list to advance the head of
 */
void list_advance_head( list_head_t* list );

/**
 * Get the next node in a list
 * @param TYPE The type of the list nodes
 * @param MEMBER The name of the list_node_t member
 * @param CURRENT The node whos successor to find
 * @return The provided nodes successor
 */
#define LIST_GET_NEXT( TYPE, MEMBER, CURRENT ) ( CONTAINER_OF( TYPE, MEMBER, list_get_next_node( &((CURRENT)->MEMBER) ) ) )
/**
 * Get the previous node in a list
 * @param TYPE The type of the list nodes
 * @param MEMBER The name of the list_node_t member
 * @param CURRENT The node whos predecessor to find
 * @return The provided nodes predecessor
 */
#define LIST_GET_PREV( TYPE, MEMBER, CURRENT ) ( CONTAINER_OF( TYPE, MEMBER, list_get_prev_node( (CURRENT)->MEMBER ) ) )

/**
 * Get the first node in a list
 * @param TYPE The type of the list nodes
 * @param MEMBER The name of the list_node_t member
 * @param HEAD The list object to get the first node of
 * @return The first node
 */
#define LIST_GET_FIRST( TYPE, MEMBER, HEAD ) ( list_is_empty( HEAD ) ? NULL : CONTAINER_OF( TYPE, MEMBER, (HEAD)->head.next ) )
/**
 * Get the last node in a list
 * @param TYPE The type of the list nodes
 * @param MEMBER The name of the list_node_t member
 * @param HEAD The list object to get the last node of
 * @return The last node
 */
#define LIST_GET_LAST( TYPE, MEMBER, HEAD ) ( list_is_empty( HEAD ) ? NULL : CONTAINER_OF( TYPE, MEMBER, (HEAD)->head.prev ) )

/**
 * Iterates through all the items in the list
 * @param TYPE The type of the list nodes
 * @param MEMBER The name of the list_node_t member
 * @param LOOP_VAR The name of the loop variable to use
 * @param HEAD The list object to iterate
 */
#define LIST_FOREACH( TYPE, MEMBER, LOOP_VAR, HEAD ) for( TYPE* LOOP_VAR = LIST_GET_FIRST( TYPE, MEMBER, HEAD ); LOOP_VAR && &(LOOP_VAR->MEMBER) != &((HEAD)->head); LOOP_VAR = LIST_GET_NEXT( TYPE, MEMBER, LOOP_VAR ) )

#endif //OS_LIST_H
