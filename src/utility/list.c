#include "list.h"
#include "utility/debug.h"

void init_list( list_node_head_t* list )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );

    list->head.next = &list->head;
    list->head.prev = &list->head;
}

bool list_is_empty( list_node_head_t* list )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );

    return list->head.next == &list->head;
}


list_node_t* list_get_next_node( list_node_t* curr )
{
    KERNEL_ASSERT( curr != NULL, "Tried to access NULL list" );
    return curr->next;
}

list_node_t* list_get_prev_node( list_node_t* curr )
{
    KERNEL_ASSERT( curr != NULL, "Tried to access NULL list" );
    return curr->prev;
}

void list_insert_after_node( list_node_t* node, list_node_t* to_insert )
{
    KERNEL_ASSERT( node != NULL, "Tried to access NULL list" );
    KERNEL_ASSERT( to_insert != NULL, "Tried to access NULL list" );

    to_insert->next = node->next;
    to_insert->prev = node;
    to_insert->next->prev = to_insert;
    to_insert->prev->next = to_insert;
}

void list_insert_before_node( list_node_t* node, list_node_t* to_insert )
{
    KERNEL_ASSERT( node != NULL, "Tried to access NULL list" );
    KERNEL_ASSERT( to_insert != NULL, "Tried to access NULL list" );

    to_insert->next = node;
    to_insert->prev = node->prev;
    to_insert->next->prev = to_insert;
    to_insert->prev->next = to_insert;
}

void list_insert_tail_node( list_node_head_t* list, list_node_t* node )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );
    KERNEL_ASSERT( node != NULL, "Tried to access NULL list" );

    // Insert at the tail of the list
    list_insert_before_node( &list->head, node );
}

void list_insert_head_node( list_node_head_t* list, list_node_t* node )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );
    KERNEL_ASSERT( node != NULL, "Tried to access NULL list" );

    // Insert at the tail of the list
    list_insert_after_node( &list->head, node );
}

void list_remove_node( list_node_t* node )
{
    KERNEL_ASSERT( node != NULL, "Tried to access NULL list" );
    KERNEL_ASSERT( node->next == node, "Cannot remove head node from list" );

    node->prev->next = node->next;
    node->next->prev = node->prev;
}

list_node_t* list_pop_head( list_node_head_t* list )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );

    if( list_is_empty( list ) )
        return NULL;

    list_node_t* head = list->head.next;
    list_remove_node( head );
    return head;
}

list_node_t* list_pop_tail( list_node_head_t* list )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );

    if( list_is_empty( list ) )
        return NULL;

    list_node_t* tail = list->head.prev;
    list_remove_node( tail );
    return tail;
}

void list_replace_node( list_node_t* old, list_node_t* new )
{
    KERNEL_ASSERT( old != NULL, "Tried to access NULL list" );
    KERNEL_ASSERT( new != NULL, "Tried to access NULL list" );

    old->prev->next = new;
    old->next->prev = old;
    new->next = old->next;
    new->prev = old->prev;
}

void list_advance_head( list_node_head_t* list )
{
    KERNEL_ASSERT( list != NULL, "Tried to access NULL list" );

    if( list_is_empty( list ) )
        return;

    list_insert_tail_node( list, list_pop_head( list ) );
}

