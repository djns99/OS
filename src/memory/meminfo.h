#ifndef OS_MEMMAP_H
#define OS_MEMMAP_H
#include "utility/types.h"

bool meminfo_phys_page_is_kernel( uint32_t page_id );
size_t get_mem_size();
void init_page_map();
size_t page_id_to_phys_address( uint32_t page_id );
uint32_t phys_address_to_page_id( size_t address );
extern uint8_t* get_kernel_start();
extern uint8_t* get_kernel_end();
extern size_t get_phys_kernel_start();
extern size_t get_phys_kernel_end();

#endif //OS_MEMMAP_H
