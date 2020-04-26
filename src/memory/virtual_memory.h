#ifndef OS_VIRTUAL_MEMORY_H
#define OS_VIRTUAL_MEMORY_H
#include "memory.h"

#define PAGE_PRESENT_FLAG (1u<<0u)
#define PAGE_MODIFIABLE_FLAG (1u<<1u)
#define PAGE_USER_ACCESSIBLE_FLAG (1u<<2u)

extern page_directory_t root_page_directory;

void init_virtual_memory();
void* alloc_any_virtual_page( uint32_t flags );
bool alloc_page_at_address( void* address, uint32_t flags );

void* kernel_heap_start();
void* kernel_heap_end();
bool kalloc_page_at_address( void* address, uint32_t flags );

void free_page( void* page );
void kfree_page( void* page );


#endif //OS_VIRTUAL_MEMORY_H
