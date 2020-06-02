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

bool init_virtual_heap( virtual_heap_t* heap, void* start_addr, void* end_addr, page_alloc_func_t page_alloc_func,
                        page_free_func_t page_free_func, uint32_t alloc_flags );
void* virtual_heap_alloc( virtual_heap_t* heap, uint32_t size );

typedef enum {
    heap_free_ok, heap_free_oom, heap_free_fatal
} heap_free_res_t;

heap_free_res_t virtual_heap_free( virtual_heap_t* heap, void* addr );

#endif //OS_VIRTUAL_HEAP_H
