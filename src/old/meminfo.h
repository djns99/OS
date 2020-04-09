#ifndef OS_MEMINFO_H
#define OS_MEMINFO_H
#include "../types.h"

#ifdef TEST_MODE
#include <stdlib.h>
#include "memory.h"

static size_t get_memory_size()
{
    // 1 GiB for tests
    return 1024*1024*1024;
}

static uint8_t* get_memory_start()
{
    static uint8_t* ptr = NULL;
    if(!ptr) ptr = (uint8_t*)aligned_alloc( SLAB_SIZE, get_memory_size() );
    // 1 GiB for tests
    return ptr;
}
#else

static size_t get_memory_size()
{
    // 1 GiB for tests
    return 0;
}

static uint8_t* get_memory_start()
{
    // 1 GiB for tests
    return 0x0;
}

#endif


#endif //OS_MEMINFO_H
