#include "utility/debug.h"
#include "virtual_memory.h"
#include "memory.h"
#include "meminfo.h"
#include "physical_memory.h"
#include "utility/string.h"

uint32_t kernel_page_dirs = 0;

void reload_page_table()
{
    // Reload cr3 to flush updates
    asm volatile( "mov %%cr3, %%eax;"
                  "mov %%eax, %%cr3;":: : "memory" );
}

bool page_directory_entry_is_valid( page_directory_ref_t page_directory, size_t virt_address )
{
    return page_directory[ virt_address / PAGE_TABLE_BYTES_ADDRESSED ] != NULL;
}

void update_page_table( page_directory_ref_t page_directory, size_t virt_address, size_t phys_address, uint32_t flags )
{
    KERNEL_ASSERT( ( phys_address % PAGE_SIZE ) == 0, "Physical address is not aligned to a page" );
    KERNEL_ASSERT( ( virt_address % PAGE_SIZE ) == 0, "Virtual address is not aligned to a page" );
    KERNEL_ASSERT( page_directory_entry_is_valid( page_directory, virt_address ), "Updated non-existant page table" );
    KERNEL_WARNING( flags & PAGE_PRESENT_FLAG, "Page allocated without present flag" );

    const size_t directory_entry = virt_address / PAGE_TABLE_BYTES_ADDRESSED;
    const size_t table_entry = ( virt_address >> PAGE_SIZE_LOG ) % PAGE_TABLE_NUM_ENTRIES;
    page_directory[ directory_entry ][ table_entry ] = (page_t*) ( phys_address | flags );

    reload_page_table();
    os_memset8( (void*) phys_address, 0x0, PAGE_SIZE );
    reload_page_table();
}

bool alloc_directory_entry( page_table_ref_t* directory_entry, uint32_t flags )
{
    KERNEL_WARNING( flags & PAGE_PRESENT_FLAG, "Page allocated without present flag" );

    uint32_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;

    KERNEL_ASSERT( ( phys_address % PAGE_SIZE ) == 0, "Physical address is not aligned to a page" );
    *directory_entry = (page_table_ref_t) ( phys_address | flags );
    reload_page_table();
    os_memset8( (void*) phys_address, 0x0, PAGE_SIZE );
    reload_page_table();

    return true;
}

uint32_t alloc_any_directory_entry( page_directory_ref_t page_directory, uint32_t flags )
{
    for( uint32_t i = 0; i < PAGE_TABLE_NUM_ENTRIES; i++ ) {
        if( ( *page_directory )[ i ] == NULL ) {
            if( !alloc_directory_entry( page_directory + i, flags ) )
                return PAGE_TABLE_NUM_ENTRIES;
            return i;
        }
    }

    return PAGE_TABLE_NUM_ENTRIES;
}

size_t find_free_virtual_address()
{
    uint32_t empty_page_directory = PAGE_TABLE_NUM_ENTRIES;
    for( uint32_t root_table_entry = 0; root_table_entry < PAGE_TABLE_NUM_ENTRIES; root_table_entry++ ) {
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
void* alloc_any_virtual_page( page_directory_ref_t page_directory, uint32_t flags )
{
    void* virt_address = (void*) find_free_virtual_address();
    if( virt_address == NULL )
        return NULL;
    if( !alloc_page_at_address( page_directory, virt_address, flags ) )
        return NULL;
    return virt_address;
}

bool alloc_page_at_address( page_directory_ref_t page_directory, void* virt_address, uint32_t flags )
{
    KERNEL_WARNING( (size_t) virt_address >= KERNEL_VIRTUAL_BASE, "Tried allocating over top of the kernel" );
    if( (size_t) virt_address >= KERNEL_VIRTUAL_BASE )
        return false;

    if( !page_directory_entry_is_valid( page_directory, (size_t) virt_address ) )
        if( !alloc_directory_entry( page_directory, flags ) )
            return false;

    size_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;
    update_page_table( page_directory, (size_t) virt_address, phys_address, flags );
    return true;
}

bool kalloc_page_at_address( page_directory_ref_t page_directory, void* virt_address, uint32_t flags )
{
    if( (size_t) virt_address >= KERNEL_VIRTUAL_BASE )
        return false;

    KERNEL_ASSERT( page_directory_entry_is_valid( page_directory, (size_t) virt_address ),
                   "Kernel heap should prealloc all entries" );

    size_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;

    update_page_table( page_directory, (size_t) virt_address, phys_address, flags );
    return true;
}

void clone_root_page_directory( page_directory_ref_t page_directory )
{
}

void init_virtual_memory()
{
    const uint32_t start_root_entry = (size_t) get_kernel_start() / PAGE_TABLE_BYTES_ADDRESSED;
    kernel_page_dirs = PAGE_TABLE_NUM_ENTRIES - start_root_entry;

    for( uint32_t i = start_root_entry; i < PAGE_TABLE_NUM_ENTRIES; i++ ) {
        // Allocate all kernel pages in the root page directory so they will be shared by all users
        if( !page_directory_entry_is_valid( root_page_directory, i ) ) {
            const bool res = alloc_directory_entry( root_page_directory + i, PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
            KERNEL_ASSERT( res, "Failed to allocate heap for kernel. Please get more memory" );
        }
    }
}