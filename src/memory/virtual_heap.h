#ifndef OS_VIRTUAL_HEAP_H
#define OS_VIRTUAL_HEAP_H
#include "utility/types.h"

typedef struct {
    
} virtual_heap_t;

void init_virtual_heap( virtual_heap_t* heap, uint32_t start_addr, uint32_t end_addr );

#endif //OS_VIRTUAL_HEAP_H
