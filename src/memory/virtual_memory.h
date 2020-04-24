#ifndef OS_VIRTUAL_MEMORY_H
#define OS_VIRTUAL_MEMORY_H
#include "memory.h"

#define PAGE_USER_ACCESSIBLE_FLAG (1u<<2)
#define PAGE_PRESENT_FLAG (1u<<0)
#define PAGE_MODIFIABLE_FLAG (1u<<1)

extern page_directory_t root_page_directory;

void init_virtual_memory();
void* alloc_any_virtual_page( page_directory_t* page_directory, uint32_t flags );
void alloc_page_at_address( page_directory_t* page_directory, void* address, uint32_t flags );

#endif //OS_VIRTUAL_MEMORY_H
