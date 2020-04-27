#include "utility/debug.h"
#include "kernel.h"
#include "utility/print.h"
#include "test_suite/test_suite.h"

static test_t test_suite[] = {
        { .name = "Fuzz Malloc Tests", .function = test_malloc_fuzz },
        { .name = "Small Malloc Tests", .function = test_malloc_small },
        { .name = "Large Malloc Tests", .function = test_malloc_large },
        { .name = "OOM Malloc Tests", .function = test_malloc_oom },
};

uint32_t successes = 0;
uint32_t failures = 0;

void test_executor()
{
    test_t test = test_suite[ OS_GetParam() ];
    print( "Running Test: %s\n", test.name );
    
    if( test.function() ) {
        uint8_t old_col = get_fg_colour();
        set_fg_colour( TEXT_L_GREEN );
        print( "Test passed\n" );
        set_fg_colour( old_col );
        successes++;
    } else {
        uint8_t old_col = get_fg_colour();
        set_fg_colour( TEXT_RED );
        print( "Test failed\n" );
        set_fg_colour( old_col );
        failures++;
    }
    
    // Signal the test is complete
    OS_Signal( TEST_RUNNER_SEMAPHORE );
}

void test_runner()
{
    const uint32_t num_tests = sizeof( test_suite ) / sizeof( test_t );
    print( "Running %u tests\n", num_tests );
    // Start from 0 since we want to wait by default
    OS_InitSem( TEST_RUNNER_SEMAPHORE, 0 );
    for( uint32_t i = 0; i < num_tests; i++ )
    {
        // Loop until a slot opens up
        OS_Create( test_executor, i, SPORADIC, 0 );
        OS_Wait( TEST_RUNNER_SEMAPHORE );
    }
    
    print( "Finished test suite\n" );
    print( "============================\n" );
    
    uint8_t old_col = get_fg_colour();
    set_fg_colour( TEXT_L_GREEN );
    print( "PASSED:\t%u\n", successes );
    set_fg_colour( TEXT_RED );
    print( "FAILED:\t%u\n", failures );
    set_fg_colour( old_col );

    print( "============================\n" );
}
