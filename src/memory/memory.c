#include "memory.h"
#include "kernel.h"
#include "utility/string.h"
#include "utility/debug.h"
#include "utility/print.h"
#include "meminfo.h"
#include "utility/bitmap.h"

extern page_t root_page_directory;

#define MAX_MEMORY_SIZE UINT32_MAX
#define MAX_NUM_PAGES (MAX_MEMORY_SIZE>>PAGE_SIZE_LOG)

DECLARE_VARIABLE_BITMAP( free_page_bitmap, MAX_NUM_PAGES );

void OS_InitMemory()
{
    init_page_map();
    print("%u\n", get_mem_size() );
    
    KERNEL_ASSERT( root_page_directory != NULL, "Failed to get the initial page directory" );
    KERNEL_ASSERT( get_kernel_start() != 0, "Failed to get the kernel start" );
    KERNEL_ASSERT( get_kernel_end() != 0, "Failed to get the kernel end" );
    KERNEL_ASSERT( get_phys_kernel_start() != 0, "Failed to get the physical kernel start" );
    KERNEL_ASSERT( get_phys_kernel_end() != 0, "Failed to get the physical kernel end" );
    print( "%p %p %p %p %p\n", root_page_directory, get_kernel_start(), get_kernel_end(), get_phys_kernel_start(), get_phys_kernel_end() );

    const uint32_t max_usable_pages = get_mem_size() >> PAGE_SIZE_LOG;
    init_variable_bitmap( free_page_bitmap, max_usable_pages );
    bitmap_clear_all( free_page_bitmap );
    
    for( uint32_t i = 0; i < max_usable_pages; i++ )
    {
        bool res = meminfo_phys_page_is_valid( i );
        bitmap_assign_bit( free_page_bitmap, i, res );
    }
}

MEMORY OS_Malloc( int val )
{
}

BOOL OS_Free( MEMORY m )
{
}

