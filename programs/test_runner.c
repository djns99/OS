#include "utility/debug.h"
#include "kernel.h"
#include "utility/print.h"
#include "test_suite/test_suite.h"
#include "sync/semaphore.h"

static test_t test_suite[] = {
        // Malloc tests
        { .name = "Small Malloc Tests", .function = test_malloc_small },
        { .name = "Large Malloc Tests", .function = test_malloc_large },
        { .name = "Malloc Underflow Regression Tests", .function = test_malloc_underflow_regression },
        { .name = "OOM Malloc Tests", .function = test_malloc_oom },
        { .name = "Many Malloc Tests", .function = test_malloc_many },
        { .name = "Variable Malloc Tests", .function = test_malloc_variable },
        { .name = "Fill Malloc Tests", .function = test_malloc_fill },
        { .name = "Interleaved Malloc Tests", .function = test_malloc_interleaved },
        { .name = "Bad Free Tests", .function = test_bad_free },

        // Semaphore tests
        { .name = "Mutual Exclusion Semaphore Tests", .function = test_semaphore_mutex },
        { .name = "N-Way Exclusion Semaphore Tests", .function = test_semaphore_n_blocked },
        { .name = "Semaphore Compliance Tests", .function = test_semaphore_compliance },

        // FIFO
        { .name = "SPSC FIFO Tests", .function = test_spsc_fifo },
        { .name = "MPSC FIFO Tests", .function = test_mpsc_fifo },
        { .name = "MPMC FIFO Tests", .function = test_mpmc_fifo },
        { .name = "Invalid FIFO Tests", .function = test_invalid_fifo },

        // Process Scheduling
        { .name = "Periodic Scheduling Tests", .function = test_periodic_scheduling },
        { .name = "Sporadic Scheduling Tests", .function = test_sporadic_scheduling },
        { .name = "Device Scheduling Tests", .function = test_device_scheduling },

        // End
};

// Programs can use globals to communicate since they compiled with the kernel
// Which means they share the kernel address space
uint32_t successes = 0;
uint32_t failures = 0;

bool watchdog_check = false;
bool watchdog_running = true;
bool watchdog_exited = true;
bool verbose = false;
bool break_on_failure = false;

// A flag used by tests to indicate non fatal failures
bool error_flag;

void test_executor()
{
    test_t test = test_suite[ OS_GetParam() ];
    if( verbose )
        print( "Running Test: %s\n", test.name );

    watchdog_check = false;
    error_flag = false;
    if( test.function() && !error_flag ) {
        uint8_t old_col = get_fg_colour();
        set_fg_colour( TEXT_L_GREEN );
        print( "%s passed\n", test.name );
        set_fg_colour( old_col );
        successes++;
    } else {
        uint8_t old_col = get_fg_colour();
        set_fg_colour( TEXT_RED );
        print( "%s failed\n", test.name );
        set_fg_colour( old_col );
        failures++;
    }

    // Signal the test is complete
    OS_Signal( TEST_RUNNER_SEMAPHORE );
}

const uint32_t watch_dog_seconds = 20;

void test_watchdog()
{
    watchdog_exited = false;
    watchdog_check = false;
    while( watchdog_running ) {
        // Assume test has died
        if( watchdog_check ) {
            print( "Test has not completed within timeout\nAssuming fatal failure\n" );
            failures++;
            watchdog_check = false;
            // Set the compliance back to relaxed in case the test modified it
            set_sem_compliance( RELAXED );
            OS_Signal( TEST_RUNNER_SEMAPHORE );
        } else {
            watchdog_check = true;
        }
        OS_Yield();
    }
    if( verbose )
        print( "Test watchdog terminated\n" );

    watchdog_exited = true;
}

void test_runner()
{
    // Used relaxed compliance mode for the tests
    semaphore_compliance_mode_t comp_mode = get_sem_compliance();
    set_sem_compliance( RELAXED );

    const uint32_t num_tests = sizeof( test_suite ) / sizeof( test_t );
    if( verbose )
        print( "Running %u tests\n", num_tests );

    // Start from 0 since we want to wait by default
    OS_InitSem( TEST_RUNNER_SEMAPHORE, 0 );

    // Check for a watch dog from a previous run
    watchdog_running = true;
    if( !watchdog_exited ) {
        watchdog_exited = false;
        PID res = OS_Create( test_watchdog, 0, DEVICE, watch_dog_seconds * 1000 );
        PROCESS_WARNING( res != INVALIDPID, "Failed to start watchdog process" );
    }

    for( uint32_t i = 0; i < num_tests; i++ ) {
        if( verbose )
            print( "Launching test %s\n", test_suite[ i ].name );

        // Launch the next test application
        PID pid = OS_Create( test_executor, i, SPORADIC, 0 );
        if( pid == INVALIDPID )
            print( "Failed to launch test %s. Skipping\n", test_suite[ i ].name );
        else
            OS_Wait( TEST_RUNNER_SEMAPHORE );

        // Yield to make sure the running test gets a chance to exit
        OS_Yield();

        if( break_on_failure && failures > 0 )
            break;
    }

    if( verbose )
        print( "Finished test suite\n" );
    print( "============================\n" );

    uint8_t old_col = get_fg_colour();
    set_fg_colour( TEXT_L_GREEN );
    print( "PASSED:\t%u\n", successes );
    set_fg_colour( TEXT_RED );
    print( "FAILED:\t%u\n", failures );
    set_fg_colour( old_col );

    successes = 0;
    failures = 0;

    print( "============================\n" );

    // Stop the watchdog
    watchdog_running = false;

    // Restore the old compliance mode
    set_sem_compliance( comp_mode );
}
