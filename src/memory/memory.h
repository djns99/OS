#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include "utility/types.h"
#include "utility/list.h"

#define PAGE_SIZE_LOG 12u
#define PAGE_SIZE (1u<<PAGE_SIZE_LOG)
typedef uint8_t page_t[PAGE_SIZE];

#define PAGE_TABLE_NUM_ENTRIES (PAGE_SIZE / sizeof( page_t* ))
#define PAGE_TABLE_BYTES_ADDRESSED (PAGE_TABLE_NUM_ENTRIES * PAGE_SIZE)
typedef page_t* page_table_t[PAGE_TABLE_NUM_ENTRIES];
typedef page_table_t* page_directory_t[PAGE_TABLE_NUM_ENTRIES];


// Kernel gets upper quarter
#define MAX_TOTAL_MEMORY_SIZE UINT32_MAX
#define MAX_KERNEL_MEMORY_SIZE (MAX_TOTAL_MEMORY_SIZE >> 2)
#define MAX_USER_MEMORY_SIZE (MAX_TOTAL_MEMORY_SIZE - MAX_KERNEL_MEMORY_SIZE)

#define MAX_TOTAL_NUM_PAGES (MAX_TOTAL_MEMORY_SIZE>>PAGE_SIZE_LOG)
#define MAX_KERNEL_NUM_PAGES (MAX_KERNEL_MEMORY_SIZE>>PAGE_SIZE_LOG)
#define MAX_USER_NUM_PAGES (MAX_USER_MEMORY_SIZE>>PAGE_SIZE_LOG)

typedef struct {
    page_directory_t* page_directory;
    list_node_head_t alloc_list[30];
    list_node_head_t free_list;
} process_memory_state_t;

void* kmalloc( uint32_t bytes );
void kfree( void* );

#endif //OS_MEMORY_H
