#ifndef OS_LIST_H
#define OS_LIST_H

#include "types.h"

#define OFFSET_OF( TYPE, MEMBER ) ((size_t)&(((TYPE*)0)->MEMBER))
#define CONTAINER_OF( TYPE, MEMBER, NODE ) (TYPE*)(((uint8_t*)node_ptr) - OFFSET_OF( TYPE, MEMBER ))

typedef struct list_node_t {
    struct list_node_t* next;
    struct list_node_t* prev;
} list_node_t;

typedef struct {
    list_node_t head;
} list_node_head_t;

void init_list( list_node_head_t* list );

bool list_is_empty( list_node_head_t* list );

list_node_t* list_get_next_node( list_node_t* curr );

list_node_t* list_get_prev_node( list_node_t* curr );

void list_insert_after_node( list_node_t* node, list_node_t* to_insert );

void list_insert_before_node( list_node_t* node, list_node_t* to_insert );

void list_insert_tail_node( list_node_head_t* list, list_node_t* node );

void list_insert_head_node( list_node_head_t* list, list_node_t* node );

void list_remove_node( list_node_t* node );

list_node_t* list_pop_head( list_node_head_t* list );

list_node_t* list_pop_tail( list_node_head_t* list );

void list_replace_node( list_node_t* old, list_node_t* new );

void list_advance_head( list_node_head_t* list );

#define LIST_GET_NEXT( TYPE, MEMBER, CURRENT ) ( CONTAINER_OF( TYPE, MEMBER, list_get_next_node( CURRENT->MEMBER ) )
#define LIST_GET_PREV( TYPE, MEMBER, CURRENT ) ( CONTAINER_OF( TYPE, MEMBER, list_get_prev_node( CURRENT->MEMBER ) ) )
#define LIST_GET_FIRST( TYPE, MEMBER, HEAD ) ( list_is_empty( HEAD ) ? NULL : CONTAINER_OF( TYPE, MEMBER, HEAD->head.next ) )
#define LIST_GET_LAST( TYPE, MEMBER, HEAD ) ( list_is_empty( HEAD ) ? NULL : CONTAINER_OF( TYPE, MEMBER, HEAD->head.prev ) )

#define LIST_FOREACH( TYPE, MEMBER, LOOP_VAR, HEAD ) for( TYPE* LOOP_VAR = LIST_GET_FIRST( TYPE, MEMBER, HEAD ); LOOP_VAR != &HEAD->head; LOOP_VAR = LIST_GET_NEXT( TYPE, MEMBER, LOOP_VAR ) )

#endif //OS_LIST_H
