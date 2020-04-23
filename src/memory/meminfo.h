#ifndef OS_MEMMAP_H
#define OS_MEMMAP_H
#include "utility/types.h"

bool meminfo_phys_page_is_valid( uint32_t page_id );
uint32_t get_mem_size();
extern uint8_t* get_kernel_start();
extern uint8_t* get_kernel_end();
extern uint8_t* get_phys_kernel_start();
extern uint8_t* get_phys_kernel_end();

#endif //OS_MEMMAP_H
