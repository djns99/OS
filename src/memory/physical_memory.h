#ifndef OS_PHYSICAL_MEMORY_H
#define OS_PHYSICAL_MEMORY_H
#include "utility/types.h"

/**
 * Initialises the physical page mappings.
 */
void init_physical_memory();

/**
 * Get a free physical page
 * @return An allocated page or NULL on failure
 */
size_t alloc_phys_page();

/**
 * Release a physical page
 * @param address The address of the physical page
 */
void free_phys_page( size_t address );

#endif //OS_PHYSICAL_MEMORY_H
