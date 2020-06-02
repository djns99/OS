#ifndef OS_PHYSICAL_MEMORY_H
#define OS_PHYSICAL_MEMORY_H
#include "utility/types.h"

void init_physical_memory();
size_t alloc_phys_page();
void free_phys_page( size_t address );

#endif //OS_PHYSICAL_MEMORY_H
