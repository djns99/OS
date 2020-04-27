#include "utility/math.h"
#include "utility/debug.h"
#include "meminfo.h"
#include "memory.h"

#define USABLE_RAM 1
#define RESERVED_RAM 1

extern const uint32_t memmap_num_entries;
typedef struct {
    uint64_t start;
    uint64_t length;
    uint32_t type;
    uint32_t flags;
} __attribute__((packed)) memmap_entry_t;
extern const memmap_entry_t memmap_table[1024];

typedef struct {
    size_t start;
    size_t length;
} filtered_memmap_entry_t;
uint32_t num_filtered_entries = 0;
filtered_memmap_entry_t filtered_memmap_table[1024];

size_t usable_ram = 0;

size_t get_mem_size()
{
    KERNEL_ASSERT( usable_ram != 0, "Called mem size before page map was initialised" );
    return usable_ram;
}

bool address_is_in_range( filtered_memmap_entry_t entry, size_t address )
{
    return entry.start <= address && address - entry.start < entry.length;
}

void init_page_map()
{
    KERNEL_ASSERT( memmap_table != NULL, "Memory map was not linked" );
    if( memmap_num_entries == 0xdeadbeef ) {
        print( "BiOS failed to load memory map. I dont know what to do here\n" );
        OS_Abort();
        return;
    }

    for( uint32_t i = 0; i < memmap_num_entries; i++ ) {

        if( memmap_table[ i ].start > MAX_PHYSICAL_MEMORY_SIZE )
            continue;

        print( "| 0x%X - 0x%X ( Length %U Type %u ) |\n", memmap_table[ i ].start,
               memmap_table[ i ].start + memmap_table[ i ].length - 1,
               memmap_table[ i ].length, memmap_table[ i ].type );
        if( memmap_table[ i ].type == USABLE_RAM ) {
            uint32_t start = (uint32_t) memmap_table[ i ].start;
            uint32_t length = (uint32_t) memmap_table[ i ].length;

            // Cap to our ram limit
            if( memmap_table[ i ].start + memmap_table[ i ].length > MAX_PHYSICAL_MEMORY_SIZE )
                length = MAX_PHYSICAL_MEMORY_SIZE - memmap_table[ i ].start;

            if( start == 0 ) {
                // Disallow zero
                start += PAGE_SIZE;
                length -= PAGE_SIZE;
            }
            
            // Round to page boundaries
            const uint32_t rounded_start = CEIL_DIV( start, PAGE_SIZE ) * PAGE_SIZE;
            length -= rounded_start - start;
            start = rounded_start;
            length -= length % PAGE_SIZE;

            filtered_memmap_table[ num_filtered_entries ].start = start;
            filtered_memmap_table[ num_filtered_entries ].length = length;
            
            usable_ram += length;
            num_filtered_entries++;
        }
    }
}

size_t page_id_to_phys_address( uint32_t page_id )
{
    uint32_t offset_left = page_id * PAGE_SIZE;
    for( uint32_t i = 0; i < num_filtered_entries; i++ ) {
        if( filtered_memmap_table[ i ].length > offset_left )
            return filtered_memmap_table[ i ].start + offset_left;
        offset_left -= filtered_memmap_table[ i ].length;
    }
    return NULL;
}

uint32_t phys_address_to_page_id( size_t address )
{
    uint32_t accumulated_offset = 0;
    for( uint32_t i = 0; i < num_filtered_entries; i++ ) {
        if( address_is_in_range( filtered_memmap_table[ i ], address ) )
            return ( accumulated_offset + ( address - filtered_memmap_table[ i ].start ) ) >> PAGE_SIZE_LOG;

        accumulated_offset += filtered_memmap_table[ i ].length;
    }
    return NULL;
}

bool meminfo_phys_page_is_kernel( uint32_t page_id )
{
    const size_t effective_address = page_id_to_phys_address( page_id );
    if( get_phys_kernel_start() <= effective_address && effective_address < get_phys_kernel_end() )
        return false; // Overlaps with kernel not allowed to alloc

    return true;
}