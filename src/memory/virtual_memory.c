#include "utility/math.h"
#include "utility/debug.h"
#include "virtual_memory.h"
#include "memory.h"
#include "meminfo.h"
#include "physical_memory.h"
#include "utility/memops.h"

// Final 4 MiB maps page directory
page_directory_ref_t const curr_page_directory = (page_directory_ref_t) 0xFFFFF000u;
page_table_t* const page_tables = (page_table_t*) MAX_ADDRESSABLE_MEMORY_SIZE;

page_table_ref_t get_page_table( uint32_t directory_entry )
{
    return page_tables[ directory_entry ];
}

phys_page_t** get_table_entry( uint32_t directory_entry, uint32_t table_entry )
{
    return page_tables[ directory_entry ] + table_entry;
}

//void reload_page_table()
//{
//    // Reload cr3 to flush updates
//    asm volatile( "mov %%cr3, %%eax;"
//                  "mov %%eax, %%cr3;":: : "memory" );
//}

void reload_page_table_address( uint32_t address )
{
    asm volatile("invlpg (%0)"::"r"(address) : "memory");
}

void reload_page_table_page( uint32_t page_id )
{
    reload_page_table_address( page_id * PAGE_SIZE );
}

void reload_page_table_address_range( uint32_t start_address, uint32_t end_address )
{
    for( uint32_t curr_page = start_address >> PAGE_SIZE_LOG; curr_page < ( end_address >> PAGE_SIZE_LOG );
         curr_page++ )
        reload_page_table_page( curr_page );
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
    KERNEL_ASSERT( page_directory_entry_is_valid( directory_entry ), "Updated non-existent page table" );
    KERNEL_WARNING( flags & PAGE_PRESENT_FLAG, "Page allocated without present flag" );

    // Update the directory flags
    curr_page_directory[ directory_entry ] = (page_table_ref_t) ( (size_t) curr_page_directory[ directory_entry ] |
                                                                  flags );

    // Update directory entry
    *get_table_entry( directory_entry, table_entry ) = (phys_page_t*) ( phys_address | flags );
}

void wipe_page_table_entry( size_t virt_address )
{
    KERNEL_ASSERT( ( virt_address % PAGE_SIZE ) == 0, "Virtual address is not aligned to a page" );
    const size_t directory_entry = address_to_directory_entry( virt_address );
    const size_t table_entry = address_to_table_entry( virt_address );

    KERNEL_ASSERT( page_directory_entry_is_valid( directory_entry ), "Freed unallocated page" );
    KERNEL_ASSERT( page_tables[ directory_entry ][ table_entry ], "Freed unallocated page" );

    // TODO Free directory entry
    const size_t phys_page = (size_t) page_tables[ directory_entry ][ table_entry ] & PAGE_MASK;
    page_tables[ directory_entry ][ table_entry ] = 0x0;

    bool dir_empty = true;
    for( uint32_t i = 0; i < PAGE_TABLE_NUM_ENTRIES; i++ ) {
        if( page_tables[ directory_entry ] ) {
            dir_empty = false;
            break;
        }
    }

    if(dir_empty) {
        free_phys_page( (size_t)curr_page_directory[ directory_entry ] );
        curr_page_directory[ directory_entry ] = NULL;
    }

    // Free the page
    free_phys_page( phys_page );
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
    reload_page_table_address( (uint32_t) get_page_table( directory_entry ) );

    // Wipe self mapped entry
    memset8( (void*) get_page_table( directory_entry ), 0x0, PAGE_SIZE );

    return true;
}

bool alloc_any_directory_entry( uint32_t flags )
{
    for( uint32_t i = 0; i < PAGE_TABLE_NUM_ENTRIES - 1; i++ ) {
        if( i * PAGE_TABLE_BYTES_ADDRESSED >= KERNEL_VIRTUAL_BASE )
            break;

        if( curr_page_directory[ i ] == NULL ) {
            if( !alloc_directory_entry( i, flags ) )
                return false;

            return true;
        }
    }

    return false;
}

size_t find_free_user_virtual_address()
{
    for( uint32_t root_table_entry = 0; root_table_entry < PAGE_TABLE_NUM_ENTRIES - 1; root_table_entry++ ) {
        if( root_table_entry * PAGE_TABLE_BYTES_ADDRESSED >= KERNEL_VIRTUAL_BASE )
            break;
        if( curr_page_directory[ root_table_entry ] == NULL )
            continue;

        for( uint32_t page_table_entry = 0; page_table_entry < PAGE_TABLE_NUM_ENTRIES; page_table_entry++ ) {
            if( !root_table_entry && !page_table_entry )
                continue; // Dont alloc 0

            if( *get_table_entry( root_table_entry, page_table_entry ) == NULL )
                return root_table_entry * PAGE_TABLE_BYTES_ADDRESSED + page_table_entry * PAGE_SIZE;
        }
    }
    return NULL;
}

void* allocate_scratch_virt_page()
{
    void* page_dir = (void*) find_free_user_virtual_address();
    if( !page_dir ) {
        if( !alloc_any_directory_entry( PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG ) )
            return NULL;
        else
            page_dir = (page_directory_ref_t) find_free_user_virtual_address();
        KERNEL_ASSERT( page_dir, "Failed to allocate page in brand new directory entry" );
    }
    return page_dir;
}

bool alloc_page_at_address( void* virt_address_ptr, uint32_t flags )
{
    size_t virt_address = (size_t) virt_address_ptr;
    KERNEL_WARNING( virt_address < KERNEL_VIRTUAL_BASE, "Tried allocating over top of the kernel" );
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

void* alloc_any_virtual_page( uint32_t flags )
{
    void* virt_address = allocate_scratch_virt_page();
    if( virt_address == NULL )
        return NULL;
    if( !alloc_page_at_address( virt_address, flags ) )
        return NULL;
    return virt_address;
}

bool kalloc_page_at_address( void* virt_address_ptr, uint32_t flags )
{
    size_t virt_address = (size_t) virt_address_ptr;
    KERNEL_ASSERT( virt_address >= KERNEL_VIRTUAL_BASE, "Tried to allocate kernel page outside kernel heap" );
    KERNEL_ASSERT( virt_address_ptr < get_kernel_start() || virt_address_ptr >= get_kernel_end(),
                   "Tried to allocate over kernel code" );

    KERNEL_ASSERT( page_directory_entry_is_valid( address_to_directory_entry( (size_t) virt_address ) ),
                   "Kernel heap should prealloc all entries" );

    size_t phys_address = alloc_phys_page();
    if( !phys_address )
        return false;

    update_page_table( (size_t) virt_address, phys_address, flags );
    return true;
}

void free_page( void* page )
{
    KERNEL_ASSERT( (size_t) page < KERNEL_VIRTUAL_BASE, "Tried to free kernel page" );
    wipe_page_table_entry( (size_t) page );
}

void kfree_page( void* page )
{
    KERNEL_ASSERT( (size_t) page >= KERNEL_VIRTUAL_BASE, "Page is not in kernel heap" );
    wipe_page_table_entry( (size_t) page );
}

/*
 * Public functions
 */
bool alloc_address_range( void* start_address, uint32_t num_pages, uint32_t flags )
{
    // Allocate pool
    for( uint32_t i = 0; i < num_pages; i++ ) {
        if( !alloc_page_at_address( start_address + i * PAGE_SIZE, flags ) ) {
            // Free any pages we allocated
            for( uint32_t j = 0; j < i; j++ )
                free_page( start_address + j * PAGE_SIZE );

            return false;
        }
    }

    // Reload the TLB
    reload_page_table_address_range( (uint32_t) start_address, (uint32_t) start_address + num_pages * PAGE_SIZE );

    // Wipe allocated pages for security
    memset8( start_address, 0x0, num_pages * PAGE_SIZE );

    return true;
}

bool kalloc_address_range( void* start_address, uint32_t num_pages, uint32_t flags )
{
    // Allocate pool
    for( uint32_t i = 0; i < num_pages; i++ ) {
        if( !kalloc_page_at_address( start_address + i * PAGE_SIZE, flags ) ) {
            // Free any pages we allocated
            for( uint32_t j = 0; j < i; j++ )
                kfree_page( start_address + j * PAGE_SIZE );

            return false;
        }
    }

    // Reload the TLB
    reload_page_table_address_range( (uint32_t) start_address, (uint32_t) start_address + num_pages * PAGE_SIZE );

    // Wipe allocated pages for security
    memset8( start_address, 0x0, num_pages * PAGE_SIZE );

    return true;
}

void free_address_range( void* address, uint32_t num_pages )
{
    for( uint32_t i = 0; i < num_pages; i++ )
        free_page( address + i * PAGE_SIZE );

    reload_page_table_address_range( (uint32_t) address, (uint32_t) address + num_pages * PAGE_SIZE );
}

void kfree_address_range( void* address, uint32_t num_pages )
{
    for( uint32_t i = 0; i < num_pages; i++ )
        kfree_page( address + i * PAGE_SIZE );

    reload_page_table_address_range( (uint32_t) address, (uint32_t) address + num_pages * PAGE_SIZE );
}

void alloc_stack( page_directory_ref_t page_dir, size_t stack_size )
{
    KERNEL_ASSERT( stack_size <= PAGE_TABLE_BYTES_ADDRESSED, "Stack must be less than one page directory entry" );

    page_table_ref_t scratch_page_table = (page_table_ref_t) alloc_any_virtual_page(
            PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
    phys_page_t** page_directory_map_entry = get_table_entry( address_to_directory_entry( (size_t) scratch_page_table ),
                                                              address_to_table_entry( (size_t) scratch_page_table ) );
    const size_t phys_page = ( (size_t) *page_directory_map_entry ) & PAGE_MASK;

    memset8( scratch_page_table, 0x0, PAGE_SIZE );

    const uint32_t dir_ent = address_to_directory_entry( MAX_USER_MEMORY_SIZE - PAGE_SIZE );
    page_dir[ dir_ent ] = (page_table_ref_t) ( phys_page | ( PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG |
                                                             PAGE_PRESENT_FLAG ) );

    for( size_t i = 0; i < stack_size; i += PAGE_SIZE ) {
        const size_t stack_virt_page = MAX_USER_MEMORY_SIZE - PAGE_SIZE - i;
        KERNEL_ASSERT( address_to_directory_entry( stack_virt_page ) == dir_ent, "Overflowed directory entry" );
        const uint32_t tab_ent = address_to_table_entry( stack_virt_page );

        // Allocate a new physical page
        scratch_page_table[ tab_ent ] = (phys_page_t*) ( alloc_phys_page() |
                                                         ( PAGE_USER_ACCESSIBLE_FLAG | PAGE_MODIFIABLE_FLAG |
                                                           PAGE_PRESENT_FLAG ) );
    }

    // Wipe local mapping for the directory entry
    *page_directory_map_entry = NULL;
    reload_page_table_address_range( MAX_USER_MEMORY_SIZE - stack_size, MAX_USER_MEMORY_SIZE - 1 );

    // At this point the page_dir will point to a page table with many physical pages
    // It contains the only reference to those pages
}

size_t init_new_process_address_space( size_t stack_size )
{
    page_directory_ref_t new_directory = (page_directory_ref_t) alloc_any_virtual_page(
            PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG );
    if( !new_directory )
        return NULL;

    const uint32_t kernel_start_entry = KERNEL_VIRTUAL_BASE / PAGE_TABLE_BYTES_ADDRESSED;
    KERNEL_ASSERT( kernel_start_entry < PAGE_TABLE_NUM_ENTRIES, "Kernel started outside page directory!" );

    // All non kernel pages start zeroed
    memset8( new_directory, 0x0, kernel_start_entry * sizeof( page_table_ref_t ) );
    // Memcpy the kernel pages from the root directory
    memcpy( new_directory + kernel_start_entry, curr_page_directory + kernel_start_entry,
            ( PAGE_TABLE_NUM_ENTRIES - kernel_start_entry ) * sizeof( page_table_ref_t ) );

    phys_page_t** page_table_entry = get_table_entry( address_to_directory_entry( (size_t) new_directory ),
                                                      address_to_table_entry( (size_t) new_directory ) );
    const size_t phys_page = ( (size_t) *page_table_entry ) & PAGE_MASK;
    // Update the self map to refer to the new page directory
    new_directory[ PAGE_TABLE_NUM_ENTRIES - 1 ] = (page_table_ref_t) ( phys_page |
                                                                       ( PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG ) );

    // Allocate a stack for the new process
    alloc_stack( new_directory, stack_size );

    // Wipe page table entry in this address space
    // Do NOT call free_page since we want to keep our physical page
    // This was just a scratch space for setting up, new process should load the new entry into cr3 when ready
    *page_table_entry = 0x0;
    reload_page_table_address( (uint32_t) new_directory );

    return phys_page;
}

void free_page_table_memory( page_table_ref_t page_table )
{
    for( uint32_t tab_ent = 0; tab_ent < PAGE_TABLE_NUM_ENTRIES; tab_ent++ ) {
        if( !page_table[ tab_ent ] )
            continue;

        // Free the page
        const size_t used_phys_page = (size_t) page_table[ tab_ent ] & PAGE_MASK;
        free_phys_page( used_phys_page );
    }
}

bool free_process_memory( size_t page_dir_phys_addr )
{
    page_directory_ref_t page_dir = (page_directory_ref_t) allocate_scratch_virt_page();
    if( !page_dir )
        return false;

    phys_page_t** dir_page_table_entry = get_table_entry( address_to_directory_entry( (size_t) page_dir ),
                                                          address_to_table_entry( (size_t) page_dir ) );
    KERNEL_ASSERT( *dir_page_table_entry == NULL, "Allocated non-NULL virtual address" );
    *dir_page_table_entry = (phys_page_t*) ( (size_t) page_dir_phys_addr |
                                             ( PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG ) );
    reload_page_table_address( (uint32_t) page_dir );

    page_table_ref_t page_tab = (page_table_ref_t) allocate_scratch_virt_page();
    if( !page_tab ) {
        *dir_page_table_entry = NULL;
        reload_page_table_address( (uint32_t) page_dir );
        return false;
    }

    phys_page_t** tab_page_table_entry = get_table_entry( address_to_directory_entry( (size_t) page_tab ),
                                                          address_to_table_entry( (size_t) page_tab ) );
    KERNEL_ASSERT( *tab_page_table_entry == NULL, "Allocated non-NULL virtual address" );

    // Loop through all pages in user address space
    for( uint32_t dir_ent = 0; dir_ent * PAGE_TABLE_BYTES_ADDRESSED < KERNEL_VIRTUAL_BASE; dir_ent++ ) {
        // Skip free memory
        if( !page_dir[ dir_ent ] )
            continue;

        const size_t phys_addr = (size_t) page_dir[ dir_ent ] & PAGE_MASK;
        *tab_page_table_entry = (phys_page_t*) ( phys_addr | ( PAGE_MODIFIABLE_FLAG | PAGE_PRESENT_FLAG ) );
        reload_page_table_address( (uint32_t) page_tab );

        // Free any pages still allocated in the page table
        free_page_table_memory( page_tab );
        // Free the directory entry
        free_phys_page( phys_addr );
    }

    // Free page used by self map
    free_phys_page( ( (size_t) page_dir[ PAGE_TABLE_NUM_ENTRIES - 1 ] ) & PAGE_MASK );

    // Clear page entries we allocated
    *dir_page_table_entry = NULL;
    *tab_page_table_entry = NULL;
    reload_page_table_address( (uint32_t) page_tab );
    reload_page_table_address( (uint32_t) page_dir );

    // Free the actual page directory page
    free_phys_page( page_dir_phys_addr );

    return true;
}

bool cleanup_process_address_space( size_t page_dir_phys_addr )
{
    KERNEL_ASSERT( page_dir_phys_addr != ( (size_t) curr_page_directory[ PAGE_TABLE_NUM_ENTRIES - 1 ] & PAGE_MASK ),
                   "Tried to clean up address space of running process" );

    return free_process_memory( page_dir_phys_addr );
}

void init_virtual_memory()
{
    const uint32_t start_root_entry = (size_t) get_kernel_start() / PAGE_TABLE_BYTES_ADDRESSED;
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
    return (void*) ( CEIL_DIV( ( (size_t) get_kernel_end() ), PAGE_SIZE ) * PAGE_SIZE );
}

void* kernel_heap_end()
{
    return (void*) ( ( MAX_ADDRESSABLE_MEMORY_SIZE >> PAGE_SIZE_LOG ) * PAGE_SIZE );
}

bool virt_address_is_valid( void* address )
{
    const size_t dir_entry = address_to_directory_entry( (size_t) address );
    if( !page_directory_entry_is_valid( dir_entry ) )
        return false;
    const size_t tab_entry = address_to_table_entry( (size_t) address );
    return *get_table_entry( dir_entry, tab_entry ) != NULL;
}

