#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include "utility/types.h"
#include "utility/list.h"

#define PAGE_SIZE_LOG 12u
#define PAGE_SIZE (1u<<PAGE_SIZE_LOG)
typedef uint8_t page_t[PAGE_SIZE];
typedef page_t* page_table_entry_t[PAGE_SIZE / sizeof( page_t* )];

typedef struct {
    page_table_entry_t* page_directory;
    list_node_head_t alloc_list[30];
    list_node_head_t free_list;
} process_memory_state_t;

#endif //OS_MEMORY_H
