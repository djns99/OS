#ifndef OS_MEMMAP_H
#define OS_MEMMAP_H
#include "utility/types.h"

/**
 * Checks if a page is in the kernel address space
 * @param page_id The page to check
 * @return True if it is in the kernel address space, false otherwise
 */
bool meminfo_phys_page_is_kernel( uint32_t page_id );

/**
 * Get the amount of usable ram availible to the system
 * @return The number of bytes of usable RAM
 */
size_t get_mem_size();

/**
 * Initialises the page mappings for the usable RAM
 */
void init_page_map();

/**
 * Converts a page id to to a physical ram offset
 * @param page_id Page id to convert
 * @return The physical address of the page
 */
size_t page_id_to_phys_address( uint32_t page_id );
/**
 * Converts a physical offset in ram to a page id
 * @param address The address to convert
 * @return The page id associated with the address
 */
uint32_t phys_address_to_page_id( size_t address );

/**
 * Gets the virtual address the kernel starts at
 * @return The virtual address the kernel starts at
 */
extern void* get_kernel_start();

/**
 * Gets the virtual address the kernel ends at
 * @return The virtual address the kernel ends at
 */
extern void* get_kernel_end();

/**
 * Gets the physical address the kernel starts at
 * @return The physical address the kernel starts at
 */
extern size_t get_phys_kernel_start();
/**
 * Gets the physical address the kernel ends at
 * @return The physical address the kernel ends at
 */
extern size_t get_phys_kernel_end();

#endif //OS_MEMMAP_H
