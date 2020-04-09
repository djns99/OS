#include "list.h"
#include "debug.h"

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
    KERNEL_ASSERT(curr != NULL);
    return curr->next;
}

list_node_t* list_get_prev_node( list_node_t* curr )
{
    KERNEL_ASSERT(curr != NULL);
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
    KERNEL_ASSERT(node != NULL);

    // Insert at the tail of the list
    list_insert_before_node(&list->head, node);
}

void list_insert_head_node( list_node_head_t* list, list_node_t* node )
{
    KERNEL_ASSERT(node != NULL);
    // Insert at the tail of the list
    list_insert_after_node(&list->head, node);
}

void list_remove_node( list_node_t* node )
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void list_replace_node( list_node_t* old, list_node_t* new )
{
    old->prev->next = new;
    old->next->prev = old;
    new->next = old->next;
    new->prev = old->prev;
}