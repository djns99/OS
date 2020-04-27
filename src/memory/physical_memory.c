#include "interrupt/interrupt.h"
#include "physical_memory.h"
#include "utility/bitmap.h"
#include "memory.h"
#include "virtual_memory.h"
#include "meminfo.h"
#include "utility/debug.h"

DECLARE_VARIABLE_BITMAP( free_page_bitmap, MAX_TOTAL_NUM_PAGES );

void init_physical_memory()
{
    print( "%u\n", get_mem_size() );

    KERNEL_ASSERT( get_kernel_start() != 0, "Failed to get the kernel start" );
    KERNEL_ASSERT( get_kernel_end() != 0, "Failed to get the kernel end" );
    KERNEL_ASSERT( get_phys_kernel_start() != 0, "Failed to get the physical kernel start" );
    KERNEL_ASSERT( get_phys_kernel_end() != 0, "Failed to get the physical kernel end" );

    const uint32_t max_usable_pages = get_mem_size() >> PAGE_SIZE_LOG;
    init_variable_bitmap( free_page_bitmap, max_usable_pages );
    bitmap_clear_all( free_page_bitmap );

    for( uint32_t i = 0; i < max_usable_pages; i++ ) {
        bool res = meminfo_phys_page_is_kernel( i );
        bitmap_assign_bit( free_page_bitmap, i, res );
    }
}

size_t alloc_phys_page()
{
    disable_interrupts();
    const uint32_t page_id = bitmap_find_first_set( free_page_bitmap );
    if( page_id == get_bitmap_bits( free_page_bitmap ) ) {
        enable_interrupts();
        return NULL;
    }
    bitmap_clear_bit( free_page_bitmap, page_id );
    enable_interrupts();
    return page_id_to_phys_address( page_id );
}

void free_phys_page( size_t address )
{
    KERNEL_ASSERT( address, "Tried to free NULL page" );
    const uint32_t page_id = phys_address_to_page_id( address );
    KERNEL_ASSERT( page_id, "Tried to free illegal page" );
    disable_interrupts();
    bitmap_set_bit( free_page_bitmap, page_id );
    enable_interrupts();
}

