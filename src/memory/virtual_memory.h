#ifndef OS_VIRTUAL_MEMORY_H
#define OS_VIRTUAL_MEMORY_H
#include "memory.h"

#define PAGE_PRESENT_FLAG (1u<<0u)
#define PAGE_MODIFIABLE_FLAG (1u<<1u)
#define PAGE_USER_ACCESSIBLE_FLAG (1u<<2u)

void init_virtual_memory();

// Clones the template page table when creating a new process
// Returns the physical address to load into cr3
size_t init_new_process_address_space( uint32_t stack_size );
bool cleanup_process_address_space( size_t phys_page );

bool alloc_address_range( void* address, uint32_t num_pages, uint32_t flags );

void* kernel_heap_start();
void* kernel_heap_end();
bool kalloc_address_range( void* address, uint32_t num_pages, uint32_t flags );

void free_address_range( void* address, uint32_t num_pages );
void kfree_address_range( void* address, uint32_t num_pages );

#endif //OS_VIRTUAL_MEMORY_H
