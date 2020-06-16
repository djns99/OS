#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include "utility/types.h"
#include "utility/list.h"
#include "processes/process.h"

#define PAGE_SIZE_LOG 12u
#define PAGE_SIZE (1u<<PAGE_SIZE_LOG)
// Mask to drop lower bits of page address
#define PAGE_MASK (~(PAGE_SIZE - 1))
typedef uint8_t phys_page_t[PAGE_SIZE];

#define PAGE_TABLE_NUM_ENTRIES (PAGE_SIZE / sizeof( phys_page_t* ))
#define PAGE_TABLE_BYTES_ADDRESSED (PAGE_TABLE_NUM_ENTRIES * PAGE_SIZE)
typedef phys_page_t* page_table_t[PAGE_TABLE_NUM_ENTRIES];
typedef phys_page_t** page_table_ref_t;
typedef page_table_ref_t page_directory_t[PAGE_TABLE_NUM_ENTRIES];
typedef page_table_ref_t* page_directory_ref_t;


// Kernel gets upper quarter
#define KERNEL_VIRTUAL_BASE 0xC0000000u
#define MAX_PHYSICAL_MEMORY_SIZE (UINT32_MAX)
// Last page in virtual address space is reserved for page table so it is not addressable
// +1 since UINT32_MAX is 1 below 4GiB
#define MAX_ADDRESSABLE_MEMORY_SIZE (MAX_PHYSICAL_MEMORY_SIZE - PAGE_TABLE_BYTES_ADDRESSED + 1)
#define MAX_KERNEL_MEMORY_SIZE (MAX_ADDRESSABLE_MEMORY_SIZE - KERNEL_VIRTUAL_BASE)
#define MAX_USER_MEMORY_SIZE (MAX_ADDRESSABLE_MEMORY_SIZE - MAX_KERNEL_MEMORY_SIZE)

#define MAX_TOTAL_NUM_PAGES (MAX_ADDRESSABLE_MEMORY_SIZE>>PAGE_SIZE_LOG)
#define MAX_KERNEL_NUM_PAGES (MAX_KERNEL_MEMORY_SIZE>>PAGE_SIZE_LOG)
#define MAX_USER_NUM_PAGES (MAX_USER_MEMORY_SIZE>>PAGE_SIZE_LOG)

/**
 * Allocate memory from the user heap
 * @param bytes The number of bytes to allocate
 * @return A virtual pointer to the allocated memory, or NULL on failure
 */
void* user_malloc( uint32_t bytes );

/**
 * Free user memory
 * @param ptr The pointer to free
 * @return True if the memory was successfully freed, false otherwise
 */
bool user_free( void* ptr );
/**
 * Allocate memory from the kernel heapp
 * @param bytes The number of bytes to allocate
 * @return A virtual pointer to the allocated memory, or NULL on failure
 */
void* kmalloc( uint32_t bytes );
/**
 * Free kernel memory
 * @param ptr The pointer to free
 * @return True if the memory was successfully freed, false otherwise
 */
bool kfree( void* ptr );

/**
 * Initialises new process heap
 * @return True if successfully init, false otherwise
 */
bool process_init_memory( pcb_t* );

/**
 * Initialises the kernel heap
 * @return True if successfully init, false otherwise
 */
bool init_kernel_memory();

#endif //OS_MEMORY_H
