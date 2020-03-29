#ifndef OS_LIST_H
#define OS_LIST_H
#include "types.h"
#include "debug.h"

#define offsetof( TYPE, MEMBER ) ((uint32_t)&(((TYPE*)0)->MEMBER))
#define container_of( TYPE, MEMBER, NODE ) (TYPE*)(((uint8_t*)node_ptr) - offsetof( TYPE, MEMBER ))

typedef struct list_node_t
{
    struct list_node_t* next;
} list_node_t;

typedef struct {
    list_node_t* head;
} list_node_head_t;

list_node_t* list_get_next_node( list_node_t* curr )
{
    KERNEL_ASSERT( curr != NULL );
    return curr->next;
}

void list_set_next_node( list_node_t* curr, list_node_t* next )
{
    KERNEL_ASSERT( curr != NULL );
    curr->next = next;
}

void list_init_node( list_node_t* node )
{
    KERNEL_ASSERT( node != NULL );
    node->next = NULL;
}


#define list_get_next( TYPE, MEMBER, CURRENT ) ( list_get_next_node( CURRENT->MEMBER ) == NULL ? NULL : container_of( TYPE, MEMBER, list_get_next_node( CURRENT->MEMBER ) ) )
#define list_get_first( TYPE, MEMBER, HEAD ) ( HEAD->head == NULL ? NULL : container_of( TYPE, MEMBER, HEAD->head ) )
#define list_add_head( TYPE, MEMBER, HEAD, NEW ) { KERNEL_ASSERT( NEW != NULL ); NEW->next = HEAD->head; HEAD->head = NEW; }

#define LIST_FOREACH( TYPE, MEMBER, LOOP_VAR, HEAD ) for( TYPE* LOOP_VAR = list_get_first( TYPE, MEMBER, HEAD ); LOOP_VAR != NULL; LOOP_VAR = list_get_next( LOOP_VAR ) )

#endif //OS_LIST_H
