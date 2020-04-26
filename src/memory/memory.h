#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include "utility/types.h"
#include "utility/list.h"
#include "processes/process.h"

#define PAGE_SIZE_LOG 12u
#define PAGE_SIZE (1u<<PAGE_SIZE_LOG)
typedef uint8_t page_t[PAGE_SIZE];

#define PAGE_TABLE_NUM_ENTRIES (PAGE_SIZE / sizeof( page_t* ))
#define PAGE_TABLE_BYTES_ADDRESSED (PAGE_TABLE_NUM_ENTRIES * PAGE_SIZE)
typedef page_t* page_table_t[PAGE_TABLE_NUM_ENTRIES];
typedef page_t** page_table_ref_t;
typedef page_table_ref_t page_directory_t[PAGE_TABLE_NUM_ENTRIES];
typedef page_table_ref_t* page_directory_ref_t;


// Kernel gets upper quarter
#define KERNEL_VIRTUAL_BASE 0xC0000000u
// Last page in virtual address space is reserved for page table so it is not addressable
// +1 since UINT32_MAX is 1 below 4GiB 
#define MAX_TOTAL_MEMORY_SIZE (UINT32_MAX - PAGE_TABLE_BYTES_ADDRESSED + 1)
#define MAX_KERNEL_MEMORY_SIZE (MAX_TOTAL_MEMORY_SIZE - KERNEL_VIRTUAL_BASE)
#define MAX_USER_MEMORY_SIZE (MAX_TOTAL_MEMORY_SIZE - MAX_KERNEL_MEMORY_SIZE)

#define MAX_TOTAL_NUM_PAGES (MAX_TOTAL_MEMORY_SIZE>>PAGE_SIZE_LOG)
#define MAX_KERNEL_NUM_PAGES (MAX_KERNEL_MEMORY_SIZE>>PAGE_SIZE_LOG)
#define MAX_USER_NUM_PAGES (MAX_USER_MEMORY_SIZE>>PAGE_SIZE_LOG)

typedef struct {
    page_directory_ref_t page_directory;
    list_head_t alloc_list[30];
    list_head_t free_list;
} process_memory_state_t;

void* kmalloc( uint32_t bytes );
void kfree( void* );

void process_init_memory( pcb_t* );

#endif //OS_MEMORY_H
