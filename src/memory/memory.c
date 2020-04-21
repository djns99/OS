#include "memory.h"
#include "kernel.h"
#include "utility/string.h"
#include "utility/debug.h"
#include "utility/print.h"

extern page_t root_page_directory;
extern size_t kernel_start;
extern size_t kernel_end;

void OS_InitMemory()
{
    KERNEL_ASSERT( root_page_directory != NULL, "Failed to get the initial page directory" );
    KERNEL_ASSERT( kernel_start != 0, "Failed to get the kernel start" );
    print( "%p %p", root_page_directory, kernel_start );
    KERNEL_ASSERT( kernel_end != 0, "Failed to get the kernel end" );

//    // Set usable RAM to start on the first page after OS
//    memory_start_page = ( kernel_end - ( kernel_end % PAGE_SIZE ) ) + PAGE_SIZE;
//
//    print( "Usable RAM at 0x%x\n", memory_start_page );
//
//    global_page_directory = (page_table_entry_t*) memory_start_page;
//    os_memset( global_page_directory, 0x0, sizeof( page_table_entry_t ) );
}

MEMORY OS_Malloc( int val )
{
}

BOOL OS_Free( MEMORY m )
{
}

