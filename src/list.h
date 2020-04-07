#ifndef OS_LIST_H
#define OS_LIST_H
#include "types.h"
#include "debug.h"

#define OFFSET_OF( TYPE, MEMBER ) ((uint32_t)&(((TYPE*)0)->MEMBER))
#define CONTAINER_OF( TYPE, MEMBER, NODE ) (TYPE*)(((uint8_t*)node_ptr) - OFFSET_OF( TYPE, MEMBER ))

typedef struct list_node_t
{
    struct list_node_t* next;
    struct list_node_t* prev;
} list_node_t;

typedef struct {
    list_node_t head;
} list_node_head_t;

void init_list( list_node_head_t* list )
{
    list->head.next = &list->head;
    list->head.prev = &list->head;
}

bool list_is_empty( list_node_head_t* list )
{
    return list->head.next == &list->head;
}


list_node_t* list_get_next_node( list_node_t* curr )
{
    KERNEL_ASSERT( curr != NULL );
    return curr->next;
}

list_node_t* list_get_prev_node( list_node_t* curr )
{
    KERNEL_ASSERT( curr != NULL );
    return curr->prev;
}

void list_insert_after_node( list_node_t* node, list_node_t* to_insert )
{
    to_insert->next = node->next;
    to_insert->prev = node;
    to_insert->next->prev = to_insert;
    to_insert->prev->next = to_insert;
}

void list_insert_before_node( list_node_t* node, list_node_t* to_insert )
{
    to_insert->next = node;
    to_insert->prev = node->prev;
    to_insert->next->prev = to_insert;
    to_insert->prev->next = to_insert;
}

void list_insert_tail_node( list_node_head_t* list, list_node_t* node )
{
    KERNEL_ASSERT( node != NULL );
    
    // Insert at the tail of the list
    list_insert_before_node( &list->head, node );
}

void list_insert_head_node( list_node_head_t* list, list_node_t* node )
{
    KERNEL_ASSERT( node != NULL );
    // Insert at the tail of the list
    list_insert_after_node( &list->head, node );
}

#define LIST_GET_NEXT( TYPE, MEMBER, CURRENT ) ( CONTAINER_OF( TYPE, MEMBER, list_get_next_node( CURRENT->MEMBER ) )
#define LIST_GET_PREV( TYPE, MEMBER, CURRENT ) ( CONTAINER_OF( TYPE, MEMBER, list_get_prev_node( CURRENT->MEMBER ) ) )
#define LIST_GET_FIRST( TYPE, MEMBER, HEAD ) ( list_is_empty( HEAD ) ? NULL : CONTAINER_OF( TYPE, MEMBER, HEAD->head.next ) )
#define LIST_GET_LAST( TYPE, MEMBER, HEAD ) ( list_is_empty( HEAD ) ? NULL : CONTAINER_OF( TYPE, MEMBER, HEAD->head.prev ) )

#define LIST_FOREACH( TYPE, MEMBER, LOOP_VAR, HEAD ) for( TYPE* LOOP_VAR = LIST_GET_FIRST( TYPE, MEMBER, HEAD ); LOOP_VAR != &HEAD->head; LOOP_VAR = LIST_GET_NEXT( TYPE, MEMBER, LOOP_VAR ) )
    
#endif //OS_LIST_H
