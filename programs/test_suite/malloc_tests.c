#include "peripherals/timer.h"
#include "memory/memory.h"
#include "memory/meminfo.h"
#include "utility/memops.h"
#include "utility/rand.h"
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

    ASSERT_TRUE( OS_Free( (MEMORY) malloc1 ) );
    ASSERT_TRUE( OS_Free( (MEMORY) malloc2 ) );

    return true;
}

bool test_malloc_underflow_regression()
{
    uint8_t* malloc1 = (void*) OS_Malloc( 1 );
    uint8_t* malloc2 = (void*) OS_Malloc( 1 );
    uint8_t* malloc3 = (void*) OS_Malloc( 1 );

    ASSERT_NE( malloc1, NULL );
    ASSERT_NE( malloc2, NULL );
    ASSERT_NE( malloc3, NULL );

    ASSERT_TRUE( OS_Free( (MEMORY) malloc2 ) );

    uint8_t* malloc4 = (void*) OS_Malloc( 1 );

    ASSERT_TRUE( OS_Free((MEMORY)malloc1) );
    ASSERT_TRUE( OS_Free((MEMORY)malloc3) );
    ASSERT_TRUE( OS_Free((MEMORY)malloc4) );

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

    memset8( malloc1, 0x1, alloc_size );
    memset8( malloc2, 0x1, alloc_size );

    ASSERT_EQ( *malloc1, 1 );
    ASSERT_EQ( *malloc2, 1 );
    ASSERT_TRUE( memcmp( malloc1, malloc2, alloc_size ) );

    memset8( malloc2, 0x2, alloc_size );
    ASSERT_FALSE( memcmp( malloc1, malloc2, alloc_size ) );

    for(int i = 0; i < alloc_size; i++) {
        ASSERT_EQ(malloc1[i], 0x1);
        ASSERT_EQ(malloc2[i], 0x2);
    }

    ASSERT_TRUE( OS_Free( (MEMORY) malloc1 ) );
    ASSERT_TRUE( OS_Free( (MEMORY) malloc2 ) );

    OS_Yield();

    uint8_t* malloc3 = (void*) OS_Malloc( alloc_size );
    uint8_t* malloc4 = (void*) OS_Malloc( alloc_size );

    ASSERT_EQ( malloc1, malloc3 );
    ASSERT_EQ( malloc2, malloc4 );

    ASSERT_TRUE( OS_Free( (MEMORY) malloc3 ) );
    ASSERT_TRUE( OS_Free( (MEMORY) malloc4 ) );

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
        ASSERT_TRUE( OS_Free( (MEMORY) malloc1 ) );

    return true;
}

bool test_malloc_many()
{
    uint8_t* allocs[1024];
    const uint32_t num_allocs = sizeof( allocs ) / sizeof( allocs[ 0 ] );

    for( uint32_t alloc_size = 1; alloc_size < 1024; alloc_size += 7 ) {
        for( uint32_t i = 0; i < num_allocs; i++ ) {
            allocs[ i ] = (uint8_t*) OS_Malloc( alloc_size );
            memset8( allocs[ i ], i, alloc_size );
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
    // Repeat the test 4 times to check state is restored
    for( uint32_t pass = 0; pass < 4; pass++ ) {
        for( uint32_t i = 0; i < num_allocs; i++ ) {
            allocs[ i ] = (uint8_t*) OS_Malloc( i + 1 );
            ASSERT_NE( allocs[ i ], NULL );
            memset8( allocs[ i ], i, i + 1 );
        }

        for( uint32_t i = 0; i < num_allocs; i++ ) {
            ASSERT_EQ( allocs[ i ][ 0 ], (uint8_t) i );
            ASSERT_EQ( allocs[ i ][ i ], (uint8_t) i );
            ASSERT_TRUE( OS_Free( (MEMORY) allocs[ i ] ) );
        }
    }

    return true;
}

bool test_malloc_fill()
{
    uint8_t* allocs[( MAX_USER_MEMORY_SIZE / 18691 ) + 1];
    // Repeat the test 4 times to check state is restored
    for( uint32_t pass = 0; pass < 4; pass++ ) {
        uint32_t i = 0;
        while( ( allocs[ i ] = (uint8_t*) OS_Malloc( 18691 ) ) ) {
            memset8( allocs[ i ], i, 18691 );
            i++;
        }

        for( uint32_t j = 0; j < i; j++ ) {
            ASSERT_EQ( allocs[ j ][ 0 ], (uint8_t) j );
            ASSERT_EQ( allocs[ j ][ 18690 ], (uint8_t) j );
            ASSERT_TRUE( OS_Free( (MEMORY) allocs[ j ] ) );
        }
    }

    return true;
}

bool test_bad_free()
{
    ASSERT_FALSE( OS_Free( 0 ) );
    ASSERT_FALSE( OS_Free( 0x1000 ) );
    ASSERT_FALSE( OS_Free( UINT32_MAX ) );

    uint8_t* malloc1 = (void*) OS_Malloc( 1000 );
    ASSERT_NE( malloc1, NULL );

    ASSERT_FALSE( OS_Free( (MEMORY)malloc1 + 1 ) );
    ASSERT_FALSE( OS_Free( (MEMORY)malloc1 + 999 ) );
    ASSERT_FALSE( OS_Free( (MEMORY)malloc1 - 1 ) );
    ASSERT_FALSE( OS_Free( 0x1000 ) );

    ASSERT_TRUE( OS_Free( (MEMORY)malloc1 ) );
    ASSERT_FALSE( OS_Free( (MEMORY)malloc1 ) );

    // Allocate 3 small buffers
    uint8_t* malloc2 = (void*) OS_Malloc( 1 );
    ASSERT_NE( malloc2, NULL );
    uint8_t* malloc3 = (void*) OS_Malloc( 1 );
    ASSERT_NE( malloc3, NULL );
    uint8_t* malloc4 = (void*) OS_Malloc( 1 );
    ASSERT_NE( malloc4, NULL );

    // Free the last two
    ASSERT_TRUE( OS_Free( (MEMORY)malloc3 ) );
    ASSERT_TRUE( OS_Free( (MEMORY)malloc4 ) );

    // Allocate a large buffer over top
    uint8_t* malloc5 = (void*) OS_Malloc( 0x1000 );
    ASSERT_NE( malloc5, NULL );

    // Check we correctly wiped the magic number
    ASSERT_FALSE(OS_Free((MEMORY)malloc4));

    ASSERT_TRUE(OS_Free((MEMORY)malloc2));
    ASSERT_TRUE(OS_Free((MEMORY)malloc5));

    return true;
}

bool test_malloc_interleaved()
{
    srand( get_time_us() );
    const uint32_t max_num_allocs = 1024;
    const uint32_t max_alloc_size = 64 * 1024;
    uint8_t* allocs[max_num_allocs];
    uint32_t num_allocs = 0;
    for( uint32_t pass = 0; pass < 100; pass++ ) {
        for( ; num_allocs < max_num_allocs; num_allocs++ ) {
            int alloc_size = ( rand() % max_alloc_size ) + 1;
            allocs[num_allocs] = (uint8_t*)OS_Malloc(alloc_size);
            ASSERT_NE(allocs[num_allocs], NULL);
            const uintptr_t uptr = (uintptr_t)allocs[num_allocs];
            const uint8_t value = uptr ^ (uptr>>8u) ^ (uptr>>16u) ^ (uptr>>24u);
            memset8( allocs[num_allocs], value, alloc_size );
        }

        const uint32_t num_to_free = ( rand() % (max_num_allocs - 1) ) + 1;
        shuffle( allocs, sizeof(uint8_t*), max_num_allocs );
        for( ; num_allocs > max_num_allocs - num_to_free; num_allocs-- )
        {
            const uintptr_t uptr = (uintptr_t)allocs[num_allocs - 1];
            const uint8_t value = uptr ^ (uptr>>8u) ^ (uptr>>16u) ^ (uptr>>24u);
            ASSERT_EQ( allocs[num_allocs - 1][0], value );
            ASSERT_TRUE(OS_Free( (MEMORY)allocs[num_allocs - 1] ) );
        }
    }

    for( uint32_t i = 0; i < num_allocs; i++ )
    {
        const uintptr_t uptr = (uintptr_t)allocs[i];
        const uint8_t value = uptr ^ (uptr>>8u) ^ (uptr>>16u) ^ (uptr>>24u);
        ASSERT_EQ( allocs[i][0], value );
        ASSERT_TRUE(OS_Free( (MEMORY)allocs[i] ) );
    }

    return true;
}