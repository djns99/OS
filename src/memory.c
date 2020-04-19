#include "memory.h"
#include "kernel.h"
#include "string.h"
#include "debug.h"
#include "print.h"

#define SECTOR_SIZE 512 // TODO Put this somewhere that makes more sense

page_table_entry_t* global_page_directory;

uint32_t memory_start_page = 0;
uint32_t kernel_start = 0;
uint32_t kernel_size = 0;

void init_memory( size_t _kernel_start, size_t _kernel_num_sectors )
{
    kernel_start = _kernel_start;
    kernel_size = _kernel_num_sectors * SECTOR_SIZE;
}

void OS_InitMemory()
{
    KERNEL_ASSERT( kernel_start != 0, "Did not configure the kernel location" );
    KERNEL_ASSERT( kernel_size != 0, "Did not configure the kernel size" );
    // Set usable RAM to start directly after OS
    memory_start_page = kernel_start + kernel_size;

    print( "Usable RAM at 0x%x (0x%x + 0x%x)\n", memory_start_page, kernel_start, kernel_size );
    
    global_page_directory = (page_table_entry_t*) memory_start_page;
    os_memset( global_page_directory, 0x0, sizeof( page_table_entry_t ) );
}

MEMORY OS_Malloc( int val )
{
}

BOOL OS_Free( MEMORY m )
{
}

