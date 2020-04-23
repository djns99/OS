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

uint32_t get_mem_size()
{
    KERNEL_ASSERT( memmap_table != NULL, "Memory map was not linked" );
    if( memmap_num_entries == 0xdeadbeef ) {
        print( "BiOS failed to load memory map. I dont know what to do here\n" );
        OS_Abort();
        return 0;
    }

    print( "Memory map of size: %u\n", memmap_num_entries );
    uint32_t usable_ram_size = 0;
    for( uint32_t i = 0; i < memmap_num_entries; i++ ) {
//        print( "| %p - %p ( Length %u Type %u ) |\n", (uint32_t) memmap_table[ i ].start,
//               (uint32_t) memmap_table[ i ].start + (uint32_t) memmap_table[ i ].length,
//               (uint32_t) memmap_table[ i ].length, (uint32_t) memmap_table[ i ].type );
        usable_ram_size += ( memmap_table[ i ].type == USABLE_RAM ) ? memmap_table[ i ].length : 0;
    }

    return usable_ram_size;
}

bool meminfo_phys_page_is_valid( uint32_t page_id )
{
    const uint8_t* effective_address = (uint8_t*) ( page_id * PAGE_SIZE );
    if( get_phys_kernel_start() <= effective_address && effective_address < get_phys_kernel_end() )
        return true;

    bool found = false;
    for( uint32_t i = 0; i < memmap_num_entries; i++ ) {
        if( memmap_table[ i ].start <= (uint32_t)effective_address &&
            memmap_table[ i ].start + memmap_table[ i ].length > (uint32_t)effective_address ) {
            if( memmap_table[ i ].type != USABLE_RAM )
                return false;

            // Dont return since we can potentially have overlapping memory regions
            found = true;
        }
    }

    return found;
}