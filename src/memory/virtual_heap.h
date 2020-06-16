#ifndef OS_VIRTUAL_HEAP_H
#define OS_VIRTUAL_HEAP_H
#include "utility/types.h"
#include "utility/list.h"

typedef bool(* page_alloc_func_t)( void*, uint32_t, uint32_t );
typedef void(* page_free_func_t)( void*, uint32_t );

typedef struct {
    size_t start;
    size_t len;
    list_node_t list_node;
} range_list_entry_t;

typedef struct {
    void* start_addr;
    void* end_addr;
    page_alloc_func_t page_alloc_func;
    page_free_func_t page_free_func;
    uint32_t alloc_flags;

    list_head_t free_list;

    range_list_entry_t* node_pool;
    size_t pool_size;
    range_list_entry_t* last_pool_alloc_loc;

    // Debug variable
    // May also be useful for reaping on OOM
    uint32_t heap_usage;
} virtual_heap_t;

/**
 * Initialises a virtual heap
 * @param heap The heap object to init
 * @param start_addr The start virtual address of the heap
 * @param end_addr The end virtual address of the heap
 * @param page_alloc_func The function the heap should use to allocate virtual pages
 * @param page_free_func The function the heap should use to free virtual pages
 * @param alloc_flags The flags to use when allocating
 * @return True if the heap is successfully initialised
 */
bool init_virtual_heap( virtual_heap_t* heap, void* start_addr, void* end_addr, page_alloc_func_t page_alloc_func,
                        page_free_func_t page_free_func, uint32_t alloc_flags );

/**
 * Allocate an address from a virtual heap
 * @param heap The heap to allocate from
 * @param size The number of bytes to allocate
 * @return An allocated virtual address, or NULL on failure
 */
void* virtual_heap_alloc( virtual_heap_t* heap, uint32_t size );

typedef enum {
    heap_free_ok, heap_free_oom, heap_free_fatal
} heap_free_res_t;

/**
 * Free an element from a virtual heap
 * @param heap The heap to free from
 * @param addr The address to free
 * @return heap_free_ok if successful, heap_free_fatal on an invalid free and
 *         heap_free_oom if there was not enough resources to free the pointer
 */
heap_free_res_t virtual_heap_free( virtual_heap_t* heap, void* addr );

#endif //OS_VIRTUAL_HEAP_H
