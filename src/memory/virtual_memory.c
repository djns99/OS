#include "utility/debug.h"
#include "virtual_memory.h"
#include "memory.h"
#include "meminfo.h"
#include "physical_memory.h"

uint32_t kernel_page_dirs = 0;

void reload_page_table() 
{
    // Reload cr3 to flush updates
    asm volatile( "mov %%cr3, %%eax;"
                  "mov %%eax, %%cr3;" 
                  ::: "memory" );
}

uint32_t update_page_table( page_directory_t* page_directory, uint32_t virt_address, uint32_t phys_address, uint32_t flags )
{
    reload_page_table();
}

void alloc_page_at_address( page_directory_t* page_directory, void* virt_address, uint32_t flags )
{
    uint32_t phys_address = alloc_phys_page();

    update_page_table( page_directory, virt_address, phys_address, flags );
}

void alloc_directory_entry( page_table_t** directory_entry, uint32_t flags ) {
    uint32_t phys_address = alloc_phys_page();
    KERNEL_ASSERT( ( phys_address % PAGE_SIZE ) == 0, "Page is not aligned" );
    *directory_entry = (page_table_t*)(phys_address | flags);
    reload_page_table();    
}

void alloc_any_directory_entry( page_directory_t* page_directory, uint32_t flags )
{
    uint32_t phys_address = alloc_phys_page();
    for( uint32_t i = 0; i < PAGE_TABLE_NUM_ENTRIES; i++ )
    {
        if( page_directory[ i ] == NULL )
        {
            alloc_directory_entry( page_directory + i, flags );
            return;
        }
    }
}

size_t find_free_virtual_address()
{
    uint32_t empty_page_directory = PAGE_TABLE_NUM_ENTRIES;
    for( uint32_t root_table_entry = 0; root_table_entry < PAGE_TABLE_NUM_ENTRIES; root_table_entry++ )
    {
        if( root_page_directory[ root_table_entry ] == NULL )
        {
            if( empty_page_directory == PAGE_TABLE_NUM_ENTRIES )
                empty_page_directory = root_table_entry;
            continue;
        }

        for ( uint32_t page_table_entry = 0; page_table_entry < PAGE_TABLE_NUM_ENTRIES; page_table_entry++ )
        {
            if( !root_table_entry && !page_table_entry )
                continue; // Dont alloc 0

            if( root_page_directory[ root_table_entry ][ page_table_entry ] != 0 )
                return root_table_entry * PAGE_TABLE_BYTES_ADDRESSED + page_table_entry * PAGE_SIZE;
        }
    }
}

void* alloc_any_virtual_page( page_directory_t* page_directory )
{
    size_t virt_address = find_free_virtual_address();
    alloc_page_at_address( page_directory, virt_address );
}

void init_virtual_memory()
{
    const uint32_t start_root_entry = (size_t)get_kernel_start() / PAGE_TABLE_BYTES_ADDRESSED;
    kernel_page_dirs = PAGE_TABLE_NUM_ENTRIES - start_root_entry;

    for( uint32_t i = start_root_entry; i < PAGE_TABLE_NUM_ENTRIES; i++ )
    {
        if( root_page_directory[ i ] == NULL )
            alloc_directory_entry( root_page_directory + i );
    }
}
