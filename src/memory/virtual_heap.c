#include "utility/math.h"
#include "utility/debug.h"
#include "virtual_heap.h"
#include "memory.h"

bool realloc_pool( virtual_heap_t* heap )
{
    // Not implemented yet
    KERNEL_WARNING( false, "Tried to realloc pool which is an unsupported operation" );
    return false;
}

void init_pool_entry( range_list_entry_t* entry, size_t start, size_t len )
{
    entry->start = start;
    entry->len = len;
}

range_list_entry_t* alloc_pool_entry( virtual_heap_t* heap, size_t start, size_t len )
{
    KERNEL_ASSERT( start, "Cannot allocate at position 0" );
    KERNEL_ASSERT( len, "Cannot allocate 0 length" );
    KERNEL_ASSERT( heap->last_pool_alloc_loc, "Did not init last pool alloc location" );
    do {
        // Last one we allocated has been freed again
        if( heap->last_pool_alloc_loc->start == 0 ) {
            init_pool_entry( heap->last_pool_alloc_loc, start, len );
            return heap->last_pool_alloc_loc;
        }

        for( range_list_entry_t* curr = heap->last_pool_alloc_loc + 1; curr != heap->last_pool_alloc_loc; curr++ ) {
            if( curr->start == 0 ) {
                init_pool_entry( curr, start, len );
                heap->last_pool_alloc_loc = curr;
                return curr;
            }
        }
    } while( realloc_pool( heap ) );
    return NULL;
}

void free_pool_entry( range_list_entry_t* entry )
{
    // 0 is an illegal start so we use that to mark free
    entry->start = 0;
}

range_list_entry_t* locate_predecessor( list_head_t* free_list, size_t start )
{
    range_list_entry_t* prev = NULL;
    LIST_FOREACH( range_list_entry_t, list_node, curr, free_list ) {
        if( curr->start > start )
            return prev;
        prev = curr;
    }
    // We are the last element
    return prev;
}

void free_new_range( virtual_heap_t* heap, range_list_entry_t* first ) {
    // Round up to first full page
    const uint32_t start_page = CEIL_DIV( first->start, PAGE_SIZE );
    // Round down to last full page
    const uint32_t end_page = (first->start+ first->len) / PAGE_SIZE;
    // Loop through all freed pages
    for( uint32_t i = start_page; i < end_page; i++ )
        heap->page_free_func( (void*)(i * PAGE_SIZE) );
}

void merge_free_ranges( virtual_heap_t* heap, range_list_entry_t* first, range_list_entry_t* second ) {
    // If we start on a border free that page
    const uint32_t start_page = first->start >> PAGE_SIZE_LOG;
    const uint32_t end_page = (second->start + second->len) >> PAGE_SIZE_LOG;
    const uint32_t join_page = second->start >> PAGE_SIZE_LOG;
    if( start_page != join_page && end_page != join_page ) {
        // Free border page if it was split between both entries
        if( second->start % PAGE_SIZE )
            heap->page_free_func( (void*)(join_page * PAGE_SIZE) );
    }
    
    // Update first pointer
    first->len += second->len;
    // Free second pointer
    list_remove_node( &second->list_node );
    free_pool_entry( second );
}

heap_free_res_t insert_free_list( virtual_heap_t* heap, void* addr_ptr, size_t size )
{
    size_t addr = (size_t) addr_ptr;
    range_list_entry_t* prev = locate_predecessor( &heap->free_list, addr );
    range_list_entry_t* next = NULL;
    
    if( prev == NULL )
        next = LIST_GET_FIRST( range_list_entry_t, list_node, &heap->free_list );
    else
        next = LIST_GET_NEXT( range_list_entry_t, list_node, prev );

    if( prev && prev->start + prev->len > addr )
        return heap_free_fatal;
    
    // Allocate entry for new node
    range_list_entry_t* me = alloc_pool_entry( heap, addr, size );
    if( !me )
        return heap_free_oom;

    // Insert into list
    if( !prev )
        list_insert_head_node( &heap->free_list, &me->list_node );
    else
        list_insert_after_node( &prev->list_node, &me->list_node );
    
    // Merge with the previous node if needed
    if( prev && prev->start + prev->len == addr ) {
        merge_free_ranges( heap, prev, me );
        // Update me pointer to prev node
        me = prev;
    }
    
    // Merge with next node if needed
    if( next && next->start == addr + size )
        merge_free_ranges( heap, me, next );
    
    return heap_free_ok;
}

bool init_virtual_heap( virtual_heap_t* heap, void* start_addr, void* end_addr, page_alloc_func_t page_alloc_func,
                        page_free_func_t page_free_func, uint32_t alloc_flags )
{
    KERNEL_ASSERT( ( (size_t) start_addr % PAGE_SIZE ) == 0, "Heap must be page aligned" );
    KERNEL_ASSERT( ( (size_t) end_addr % PAGE_SIZE ) == 0, "Heap must be page aligned" );
    heap->start_addr = start_addr;
    heap->end_addr = end_addr;
    heap->page_alloc_func = page_alloc_func;
    heap->page_free_func = page_free_func;
    heap->alloc_flags = alloc_flags;
    
    init_list( &heap->free_list );

    const uint32_t heap_size = start_addr - end_addr;
    const uint32_t pool_entries_per_page = PAGE_SIZE / sizeof( range_list_entry_t );
    const uint32_t heap_num_pages = heap_size >> PAGE_SIZE_LOG;
    const uint32_t pool_num_pages = CEIL_DIV( heap_num_pages, pool_entries_per_page );

    // Allocate pool
    for( uint32_t i = 0; i < pool_num_pages; i++ ) {
        if( !heap->page_alloc_func( start_addr + i * PAGE_SIZE, alloc_flags ) ) {
            // Free any pages we allocated
            for( uint32_t j = 0; j < i; j++ )
                heap->page_free_func( start_addr + j * PAGE_SIZE );

            return false;
        }
    }
    
    heap->node_pool = start_addr;
    heap->pool_size = pool_num_pages * PAGE_SIZE;
    heap->last_pool_alloc_loc = heap->node_pool;

    range_list_entry_t* list_node = alloc_pool_entry( heap, (size_t) start_addr + heap->pool_size,
                                                      heap_size - heap->pool_size );
    KERNEL_ASSERT( list_node, "Failed to allocate list node from new pool" );
    
    list_insert_head_node( &heap->free_list, &list_node->list_node );

    heap->heap_usage = heap->pool_size;
    return true;
}

void* virtual_heap_alloc( virtual_heap_t* heap, uint32_t size )
{
    if( size == 0 )
        return NULL;
    if( size == 1 )
        size++;

    // Allocate in power 2 + 2
    const uint8_t size_log = os_log2_up( size );
    const uint32_t real_size = ( 1u << size_log ) + 2;
    heap->heap_usage += real_size;

    // First fit algorithm
    LIST_FOREACH( range_list_entry_t, list_node, curr, &heap->free_list ) {
        if( curr->len >= real_size ) {
            const uint32_t start_page = CEIL_DIV( curr->start, PAGE_SIZE );
            const uint32_t end_page = ( curr->start + real_size ) / PAGE_SIZE;
            
            // Allocate any new pages needed
            for( uint32_t i = start_page; i < end_page; i++ )
                heap->page_alloc_func( (void*)(i * PAGE_SIZE), heap->alloc_flags );
            
            uint16_t* actual_addr = (uint16_t*)curr->start;
            *actual_addr = size_log;

            // Remove the allocated range from entry
            curr->start += real_size;
            curr->len -= real_size;
            if( curr->len == 0 ) {
                list_remove_node( &curr->list_node );
                free_pool_entry( curr );
            }
            
            // Add 1 to address to skip size stored in first word
            return actual_addr + 1;
        }
    }

    heap->heap_usage -= real_size;
    return NULL;
}

heap_free_res_t virtual_heap_free( virtual_heap_t* heap, void* addr )
{
    void* actual_addr = addr - 2;
    const uint32_t size = ( 1u << *(uint16_t*) actual_addr ) + 2;
    return insert_free_list( heap, actual_addr, size );
}