#ifndef OS_VIRTUAL_MEMORY_H
#define OS_VIRTUAL_MEMORY_H
#include "memory.h"

#define PAGE_PRESENT_FLAG (1u<<0u)
#define PAGE_MODIFIABLE_FLAG (1u<<1u)
#define PAGE_USER_ACCESSIBLE_FLAG (1u<<2u)

/**
 * Initialises the virtual memory subsystem
 */
void init_virtual_memory();

/**
 * Allocates and initialises new page directory/tables for a new process
 * Clones the kernel address space from the current process and initialises a new stack
 * @param stack_size The size of the stack to allocate for the new process
 * @return A pointer to the page directory representing the new address space
 */
size_t init_new_process_address_space( uint32_t stack_size );

/**
 * Releases all the pages associated with a process
 * @param phys_page The address of the page directory of the process to cleanup
 * @return True if successfully cleaned up, false if there was not enough free memory to release the resources
 */
bool cleanup_process_address_space( size_t phys_page );

/**
 * Allocate physical pages to a user address
 * @param address The address to start allocating at
 * @param num_pages The number of pages to allocate
 * @param flags The page table flags for the allocations
 * @return True if the allocation was successful, false otherwise
 */
bool alloc_address_range( void* address, uint32_t num_pages, uint32_t flags );

/**
 * Allocate physical pages to a kernel address
 * @param address The address to start allocating at
 * @param num_pages The number of pages to allocate
 * @param flags The page table flags for the allocations
 * @return True if the allocation was successful, false otherwise
 */
bool kalloc_address_range( void* address, uint32_t num_pages, uint32_t flags );

/**
 * Free the pages associated with a user virtual address range
 * @param address The starting address to free
 * @param num_pages The number of pages to free
 */
void free_address_range( void* address, uint32_t num_pages );

/**
 * Free the pages associated with a kernel virtual address range
 * @param address The starting address to free
 * @param num_pages The number of pages to free
 */
void kfree_address_range( void* address, uint32_t num_pages );

/**
 * Get the start virtual address of the kernel heap
 * @return The start virtual address of the kernel heap
 */
void* kernel_heap_start();
/**
 * Get the end virtual address of the kernel heap
 * @return The end virtual address of the kernel heap
 */
void* kernel_heap_end();

#endif //OS_VIRTUAL_MEMORY_H
