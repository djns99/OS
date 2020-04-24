#include "memory.h"
#include "kernel.h"
#include "utility/string.h"
#include "utility/debug.h"
#include "meminfo.h"
#include "virtual_memory.h"
#include "physical_memory.h"

void OS_InitMemory()
{
    init_page_map();
    init_physical_memory();
    init_virtual_memory();
}

MEMORY OS_Malloc( int val )
{
    if( val == 0 )
        return NULL;

    // User alloc
    return 0;
}

BOOL OS_Free( MEMORY m )
{
    // User free
    return false;
}

