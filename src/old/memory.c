#include "kernel.h"
#include "helpers.h"
#include "buffer_helpers.h"
#include "debug.h"
#include "meminfo.h"
#include "memory.h"

uint32_t get_slab_elem_stride8( uint32_t slab_elem_size )
{
    return slab_elem_size + ALLOC_RESERVED_SIZE;
}

slab_elem_cb_t* get_slab_elem_control_block( slab_t* slab, uint32_t slab_elem_size, uint32_t idx )
{
    KERNEL_ASSERT( slab != NULL );

    uint32_t slab_stride16 = get_slab_elem_stride8( slab_elem_size ) >> 1;
    return ((slab_elem_cb_t*)slab) + slab_stride16;
}

MEMORY get_slab_elem( slab_t* slab, uint32_t slab_elem_size, uint32_t idx )
{
    KERNEL_ASSERT( slab != NULL );
    
    return (MEMORY)(get_slab_elem_control_block( slab, slab_elem_size, idx ) + (ALLOC_RESERVED_SIZE / 2));
}

MEMORY alloc_slab_elem( slab_t* slab, uint32_t slab_elem_size, uint32_t idx )
{
    KERNEL_ASSERT( slab != NULL );

    slab_elem_cb_t* control_block = get_slab_elem_control_block( slab, slab_elem_size, idx );
    *control_block &= ~FREE_BIT_MASK;
    return get_slab_elem( slab, slab_elem_size, idx );
}

void free_slab_elem( slab_t* slab, uint32_t slab_elem_size, uint32_t idx )
{
    KERNEL_ASSERT( slab != NULL );

    slab_elem_cb_t* control_block = get_slab_elem_control_block( slab, slab_elem_size, idx );
    *control_block |= FREE_BIT_MASK;
}

uint32_t get_slab_elem_size( slab_t* slab )
{
    KERNEL_ASSERT( slab != NULL );

    slab_elem_cb_t* cb = (uint16_t*)slab;
    return (*cb & ~FIRST_SLAB_ELEM_BIT_MASK);
}

slab_elem_cb_t* get_control_from_alloc( MEMORY alloc )
{
    KERNEL_ASSERT( alloc != NULL );

    return ((slab_elem_cb_t*)alloc) - (ALLOC_RESERVED_SIZE / 2);
}

slab_t* get_slab_from_alloc( MEMORY alloc )
{
    KERNEL_ASSERT( alloc != NULL );

    slab_elem_cb_t* cb = get_control_from_alloc( alloc );
    if( !(*cb & FIRST_SLAB_ELEM_BIT_MASK) )
        cb -= *cb / 2;
    return (slab_t*)cb;
}

uint32_t get_slab_idx_from_alloc( MEMORY alloc )
{
    KERNEL_ASSERT( alloc != NULL );

    slab_elem_cb_t* cb = get_control_from_alloc( alloc );
    if( *cb & FIRST_SLAB_ELEM_BIT_MASK )
        return 0;
    slab_t* slab = get_slab_from_alloc( alloc );
    return *cb / get_slab_elem_size( slab );
}

void alloc_slab( slab_t* slab, uint32_t new_elem_size )
{
    KERNEL_ASSERT( slab != NULL );

    uint32_t stride = get_slab_elem_stride8( new_elem_size );
    for( uint32_t slab_idx = 1; slab_idx * stride < SLAB_SIZE; slab_idx++ )
    {
        slab_elem_cb_t* cb = get_slab_elem_control_block( slab, new_elem_size, slab_idx );
        *cb = FREE_BIT_MASK | (slab_idx * stride);
    }
    slab_elem_cb_t* cb = get_slab_elem_control_block( slab, new_elem_size, 0 );
    *cb = FREE_BIT_MASK | FIRST_SLAB_ELEM_BIT_MASK | os_log2( new_elem_size );
}

void OS_InitMemory()
{
    uint8_t* const memory_start = get_memory_start();
    const uint32_t total_memory_size = get_memory_size();
    KERNEL_ASSERT( total_memory_size % sizeof( void* ) == 0 );

    const uint32_t max_num_slabs = total_memory_size / SLAB_SIZE;
    const uint32_t required_slab_lists = (MAX_CACHE_ENTRIES * max_num_slabs + 1);
    uint32_t slab_lists_size = required_slab_lists * max_num_slabs * sizeof( slab_t* );

    slab_t** slab_list_start = (slab_t**)memory_start;
    for( uint32_t i = 0; i < MAX_CACHE_ENTRIES; i++ )
    {
        memory_state.slab_cache[i].slabs = slab_list_start;
        memory_state.slab_cache[i].num_slabs = 0;
        slab_list_start += max_num_slabs;
    }
    memory_state.free_slabs.slabs = slab_list_start;
    slab_list_start += max_num_slabs;

    // TODO alignment
    memory_state.free_slabs.num_slabs = 0;
    for( uint32_t i = slab_lists_size; i < total_memory_size; i += SLAB_SIZE)
    {
        KERNEL_ASSERT( memory_start + i != NULL );
        memory_state.free_slabs.slabs[ memory_state.free_slabs.num_slabs++ ] = (slab_t*)(memory_start + i);
    }
}


MEMORY OS_Malloc( int val )
{
    if( !val )
        return 0;

    const uint32_t cache_idx = val < MIN_ALLOC_SIZE ? 0 : ( os_log2_up( val ) - MIN_ALLOC_SIZE_LOG );
    const uint32_t slab_element_size = get_slab_elem_stride8( MIN_ALLOC_SIZE << cache_idx );

    slab_list_t* cache_entry = &memory_state.slab_cache[ cache_idx ];
    if( cache_entry->num_slabs != 0 )
    {
        for( uint32_t slab_idx = 0; slab_idx < cache_entry->num_slabs; slab_idx++ )
        {
            for( uint32_t slab_entry = 0; slab_entry < SLAB_SIZE; slab_entry += slab_element_size )
            {
                slab_t* slab = cache_entry->slabs[ slab_idx ];
                uint16_t* slab_control = get_slab_elem_control_block( slab, slab_element_size, slab_entry );
                if( FREE_BIT_MASK & *slab_control )
                {
                    return alloc_slab_elem( slab, slab_element_size, slab_entry );
                }
            }
        }
    }

    // No slabs left
    if( memory_state.free_slabs.num_slabs == 0 )
        return 0;

    // No slabs in cache, allocate from free list
    slab_t* new_slab = memory_state.free_slabs.slabs[ memory_state.free_slabs.num_slabs-- ];
    alloc_slab( new_slab, slab_element_size );
    cache_entry->slabs[ cache_entry->num_slabs++ ] = new_slab;
    return alloc_slab_elem( new_slab, slab_element_size, 0 );
}

BOOL OS_Free( MEMORY m )
{
    slab_t* slab = get_slab_from_alloc( m );
    uint32_t slab_idx = get_slab_idx_from_alloc( m );
    uint32_t slab_elem_size = get_slab_elem_size( slab );
    free_slab_elem( slab, slab_elem_size, slab_idx );

    for( uint32_t slab_entry = 0; slab_entry < SLAB_SIZE; slab_entry += slab_elem_size )
    {
        uint16_t* slab_control = get_slab_elem_control_block( slab, slab_elem_size, slab_entry );
        if( !(FREE_BIT_MASK & *slab_control) )
        {
            // Slab is partially full - don't need to do anything
            return TRUE;
        }
    }

    // Slab is all free - remove from cache
    const uint32_t cache_idx = os_log2( slab_elem_size ) - MIN_ALLOC_SIZE_LOG;
    slab_list_t* cache_entry = &memory_state.slab_cache[ cache_idx ];
    for( uint32_t i = 0; i < cache_entry->num_slabs; i++ )
    {
        if( cache_entry->slabs[i] == slab )
        {
            cache_entry->slabs[ i ] = cache_entry->slabs[ --cache_entry->num_slabs ];
            return TRUE;
        }
    }

    KERNEL_ASSERT( FALSE );
    return FALSE;
}