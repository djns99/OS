#include "utility/helpers.h"
#include "utility/debug.h"
#include "virtual_memory.h"
#include "memory.h"
#include "meminfo.h"
#include "physical_memory.h"
#include "utility/string.h"

uint32_t kernel_page_dirs = 0;
// Final 4 MiB maps page directory
page_directory_ref_t const curr_page_directory = (page_directory_ref_t) 0xFFFFF000u;
page_table_t* const page_tables = (page_table_t*) MAX_TOTAL_MEMORY_SIZE;

page_table_ref_t get_page_table( uint32_t directory_entry )
{
    return page_tables[ directory_entry ];
}

page_t** get_table_entry( uint32_t directory_entry, uint32_t table_entry )
{
    return page_tables[ directory_entry ] + table_entry * sizeof( page_t* );
}

void reload_page_table()
{
    // TODO Use invlg instruction
    // Reload cr3 to flush updates
    asm volatile( "mov %%cr3, %%eax;"
                  "mov %%eax, %%cr3;":: : "memory" );
}

size_t address_to_directory_entry( size_t virt_address )
{
    return virt_address / PAGE_TABLE_BYTES_ADDRESSED;
}

size_t address_to_table_entry( size_t virt_address )
{
    return ( virt_address >> PAGE_SIZE_LOG ) % PAGE_TABLE_NUM_ENTRIES;
}

bool page_directory_entry_is_valid( size_t directory_entry )
{
    return curr_page_directory[ directory_entry ] != NULL;
}

void update_page_table( size_t virt_address, size_t phys_address, uint32_t flags )
{
    const size_t directory_entry = address_to_directory_entry( virt_address );
    const size_t table_entry = address_to_table_entry( virt_address );
    KERNEL_ASSERT( ( phys_address % PAGE_SIZE ) == 0, "Physical address is not aligned to a page" );
    KERNEL_ASSERT( ( virt_address % PAGE_SIZE ) == 0, "Virtual address is not aligned to a page" );
    KERNEL_ASSERT( page_directory_entry_is_valid( directory_entry ), "Updated non-existant page table" );
    KERNEL_WARNING( flags & PAGE_PRESENT_FLAG, "Page allocated without present flag" );

    // Update directory entry
    *get_table_entry( directory_entry, table_entry ) = (page_t*) ( phys_address | flags );

    reload_page_table();

    // Wipe allocated page for security
    os_memset8( (void*) virt_address, 0x0, PAGE_SIZE );
}

bool alloc_directory_entry( uint32_t directory_entry, uint32_t flags )
{
    KERNEL_ASSERT( !curr_page_directory[ directory_entry ], "Tried to overwrite non-NULL page directory entry" );
    KERNEL_WARNING( flags & PAGE_PRESENT_FLAG, "Page allocated without present flag" );

    uint32_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;

    KERNEL_ASSERT( ( phys_address % PAGE_SIZE ) == 0, "Physical address is not aligned to a page" );
    // Update the directory to point to new entry
    curr_page_directory[ directory_entry ] = (page_table_ref_t) ( phys_address | flags );
    reload_page_table();

    // Wipe self mapped entry
    os_memset8( (void*) get_page_table( directory_entry ), 0x0, PAGE_SIZE );
    reload_page_table();

    return true;
}

uint32_t alloc_any_directory_entry( uint32_t flags )
{
    for( uint32_t i = 0; i < PAGE_TABLE_NUM_ENTRIES - 1; i++ ) {
        if( curr_page_directory[ i ] == NULL ) {
            if( !alloc_directory_entry( i, flags ) )
                return PAGE_TABLE_NUM_ENTRIES;
            return i;
        }
    }

    return PAGE_TABLE_NUM_ENTRIES;
}

size_t find_free_virtual_address()
{
    uint32_t empty_page_directory = PAGE_TABLE_NUM_ENTRIES;
    for( uint32_t root_table_entry = 0; root_table_entry < PAGE_TABLE_NUM_ENTRIES - 1; root_table_entry++ ) {
        if( root_page_directory[ root_table_entry ] == NULL ) {
            if( empty_page_directory == PAGE_TABLE_NUM_ENTRIES )
                empty_page_directory = root_table_entry;
            continue;
        }

        for( uint32_t page_table_entry = 0; page_table_entry < PAGE_TABLE_NUM_ENTRIES; page_table_entry++ ) {
            if( !root_table_entry && !page_table_entry )
                continue; // Dont alloc 0

            if( root_page_directory[ root_table_entry ][ page_table_entry ] != 0 )
                return root_table_entry * PAGE_TABLE_BYTES_ADDRESSED + page_table_entry * PAGE_SIZE;
        }
    }
    return NULL;
}

/*
 * Public functions
 */
void* alloc_any_virtual_page( uint32_t flags )
{
    void* virt_address = (void*) find_free_virtual_address();
    if( virt_address == NULL )
        return NULL;
    if( !alloc_page_at_address( virt_address, flags ) )
        return NULL;
    return virt_address;
}

bool alloc_page_at_address( void* virt_address_ptr, uint32_t flags )
{
    size_t virt_address = (size_t) virt_address_ptr;
    KERNEL_WARNING( virt_address >= KERNEL_VIRTUAL_BASE, "Tried allocating over top of the kernel" );
    if( (size_t) virt_address >= KERNEL_VIRTUAL_BASE )
        return false;

    const uint32_t directory_entry = address_to_directory_entry( virt_address );
    if( !page_directory_entry_is_valid( directory_entry ) )
        if( !alloc_directory_entry( directory_entry, flags ) )
            return false;

    size_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;
    update_page_table( virt_address, phys_address, flags );
    return true;
}

bool kalloc_page_at_address( void* virt_address_ptr, uint32_t flags )
{
    size_t virt_address = (size_t) virt_address_ptr;
    KERNEL_ASSERT( virt_address >= KERNEL_VIRTUAL_BASE, "Tried to allocate kernel page outside kernel heap" );
    KERNEL_ASSERT( virt_address_ptr < get_kernel_start() || virt_address_ptr >= get_kernel_end(), "Tried to allocate over kernel code" );
    
    KERNEL_ASSERT( page_directory_entry_is_valid( address_to_directory_entry( (size_t) virt_address ) ),
                   "Kernel heap should prealloc all entries" );

    size_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;

    update_page_table( (size_t) virt_address, phys_address, flags );
    return true;
}

void clone_root_page_directory( page_directory_ref_t page_directory )
{
    // TODO this is scary
}

void init_virtual_memory()
{
    const uint32_t start_root_entry = (size_t) get_kernel_start() / PAGE_TABLE_BYTES_ADDRESSED;
    kernel_page_dirs = PAGE_TABLE_NUM_ENTRIES - start_root_entry;

    for( uint32_t i = start_root_entry; i < PAGE_TABLE_NUM_ENTRIES - 1; i++ ) {
        // Allocate all kernel pages in the root page directory so they will be shared by all users
        if( !page_directory_entry_is_valid( i ) ) {
            const bool res = alloc_directory_entry( i, PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
            KERNEL_ASSERT( res, "Failed to allocate heap for kernel. Please get more memory" );
        }
    }
}

void* kernel_heap_start()
{
    return (void*)(CEIL_DIV( ((size_t)get_kernel_end()), PAGE_SIZE ) * PAGE_SIZE);
}

void* kernel_heap_end()
{
    return (void*)(CEIL_DIV( MAX_TOTAL_MEMORY_SIZE, PAGE_SIZE ) * PAGE_SIZE);
}