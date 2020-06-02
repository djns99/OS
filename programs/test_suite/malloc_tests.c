#include "memory/memory.h"
#include "memory/meminfo.h"
#include "utility/memops.h"
#include "test_suite.h"
#include "test_helper.h"

bool test_malloc_small()
{
    uint8_t* malloc1 = (void*) OS_Malloc( 1 );
    uint8_t* malloc2 = (void*) OS_Malloc( 1 );

    ASSERT_NE( malloc1, NULL );
    ASSERT_NE( malloc2, NULL );
    ASSERT_NE( malloc1, malloc2 );

    *malloc1 = 1;
    *malloc2 = 2;
    ASSERT_EQ( *malloc1, 1 );
    ASSERT_EQ( *malloc2, 2 );

    OS_Free( (MEMORY) malloc1 );
    OS_Free( (MEMORY) malloc2 );

    return true;
}

bool test_malloc_large()
{
    const uint32_t alloc_size = 32 * 1024 * 1024;
    uint8_t* malloc1 = (void*) OS_Malloc( alloc_size );
    uint8_t* malloc2 = (void*) OS_Malloc( alloc_size );

    ASSERT_NE( malloc1, NULL );
    ASSERT_NE( malloc2, NULL );
    ASSERT_NE( malloc1, malloc2 );

    *malloc1 = 1;
    *malloc2 = 2;
    ASSERT_EQ( *malloc1, 1 );
    ASSERT_EQ( *malloc2, 2 );

    os_memset8( malloc1, 0x1, alloc_size );
    os_memset8( malloc2, 0x1, alloc_size );

    ASSERT_EQ( *malloc1, 1 );
    ASSERT_EQ( *malloc2, 1 );
    ASSERT_TRUE( os_memcmp( malloc1, malloc2, alloc_size ) );

    os_memset8( malloc2, 0x2, alloc_size );
    ASSERT_FALSE( os_memcmp( malloc1, malloc2, alloc_size ) );

    OS_Free( (MEMORY) malloc1 );
    OS_Free( (MEMORY) malloc2 );

    OS_Yield();

    uint8_t* malloc3 = (void*) OS_Malloc( alloc_size );
    uint8_t* malloc4 = (void*) OS_Malloc( alloc_size );

    ASSERT_EQ( malloc1, malloc3 );
    ASSERT_EQ( malloc2, malloc4 );

    OS_Free( (MEMORY) malloc3 );
    OS_Free( (MEMORY) malloc4 );

    return true;
}

bool test_malloc_oom()
{
    // Try one really large
    const uint32_t alloc_size = get_mem_size();
    uint8_t* malloc1 = (void*) OS_Malloc( alloc_size / 2 );

    ASSERT_NE( malloc1, NULL );

    uint8_t* malloc2 = (void*) OS_Malloc( alloc_size / 2 );

    // We know malloc 2 must fail
    ASSERT_EQ( malloc2, NULL );

    if( malloc1 )
        OS_Free( (MEMORY) malloc1 );

    return true;
}

bool test_malloc_many()
{
    uint8_t* allocs[1024];
    const uint32_t num_allocs = sizeof( allocs ) / sizeof( allocs[ 0 ] );

    for( uint32_t alloc_size = 1; alloc_size < 1024; alloc_size += 7 ) {
        for( uint32_t i = 0; i < num_allocs; i++ ) {
            allocs[ i ] = (uint8_t*) OS_Malloc( alloc_size );
            allocs[ i ][ 0 ] = i;
            allocs[ i ][ alloc_size - 1 ] = i;
            ASSERT_NE( allocs[ i ], NULL );
        }

        for( uint32_t i = 0; i < num_allocs; i++ ) {
            ASSERT_EQ( allocs[ i ][ 0 ], (uint8_t) i );
            ASSERT_EQ( allocs[ i ][ alloc_size - 1 ], (uint8_t) i );
            ASSERT_TRUE( OS_Free( (MEMORY) allocs[ i ] ) );
        }
    }

    return true;
}

bool test_malloc_variable()
{
    uint8_t* allocs[8192];
    const uint32_t num_allocs = sizeof( allocs ) / sizeof( allocs[ 0 ] );

    for( uint32_t i = 0; i < num_allocs; i++ ) {
        allocs[ i ] = (uint8_t*) OS_Malloc( i + 1 );
        ASSERT_NE( allocs[ i ], NULL );
        allocs[ i ][ 0 ] = i;
        allocs[ i ][ i ] = i;
    }

    for( uint32_t i = 0; i < num_allocs; i++ ) {
        ASSERT_EQ( allocs[ i ][ 0 ], (uint8_t) i );
        ASSERT_EQ( allocs[ i ][ i ], (uint8_t) i );
        ASSERT_TRUE( OS_Free( (MEMORY) allocs[ i ] ) );
    }

    return true;
}

bool test_malloc_fill()
{
    uint8_t* allocs[( MAX_USER_MEMORY_SIZE / 18691 ) + 1];

    uint32_t i = 0;
    while( ( allocs[ i ] = (uint8_t*) OS_Malloc( 18691 ) ) ) {
        allocs[ i ][ 0 ] = i;
        allocs[ i ][ 18690 ] = i;
        i++;
    }

    for( uint32_t j = 0; j < i; j++ ) {
        ASSERT_EQ( allocs[ j ][ 0 ], (uint8_t) j );
        ASSERT_EQ( allocs[ j ][ 18690 ], (uint8_t) j );
        ASSERT_TRUE( OS_Free( (MEMORY) allocs[ j ] ) );
    }

    return true;
}