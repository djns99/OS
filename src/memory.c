#include "memory.h"
#include "kernel.h"
#include "string.h"

page_table_entry_t* global_page_directory;

uint32_t memory_start_page = 0;

void init_memory( size_t start, size_t num_sectors )
{
    memory_start_page = start + num_sectors * 512;
}

void OS_InitMemory()
{
    os_memset( global_page_directory, 0x0, sizeof(page_table_entry_t) );
}

MEMORY OS_Malloc( int val )
{
}

BOOL OS_Free( MEMORY m )
{
}

