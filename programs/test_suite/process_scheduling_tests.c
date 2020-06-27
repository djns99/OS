#include "processes/process.h"
#include "utility/rand.h"
#include "peripherals/timer.h"
#include "test_helper.h"
#include "test_suite.h"
#include "utility/print.h"
#include "utility/memops.h"
#include "utility/types.h"

#define HOST_NOTIFY_SEM_SCHEDULING (MAXSEM - 3)

static void busy_sleep( uint64_t sleep_ms )
{
    uint64_t target_time = get_time_ms() + sleep_ms;
    while( get_time_ms() < target_time );
}

int cycle_len;
int p_len[MAXPROCESS];

// Define the PPP here in the test

void init_periodic_params()
{
    cycle_len = 0;
    for( int i = 0; i < PPPLen; i++ ) {
        cycle_len += PPPMax[ i ];
        if( PPP[ i ] != IDLE )
            p_len[ PPP[ i ] ] = PPPMax[ i ];
    }
}

volatile bool finished_processes[MAX_TEST_PROCESSES];

bool process_still_running( int num_to_check )
{
    for( int i = 0; i < num_to_check; i++ )
        if( !finished_processes[ i ] )
            return true;

    return false;
}

void periodic_test_func()
{
    const periodic_name_t p_name = OS_GetParam() & 0xffff;
    const periodic_name_t index = OS_GetParam() >> 16;
    const uint64_t start_time = get_time_ms();

    for( int i = 0; i < 10; i++ ) {
        // Yield current slot
        OS_Yield();

        // Check it has been an entire cycle
        EXPECT_GE( get_time_ms(), start_time + cycle_len * i - p_len[ p_name ] * 2 );
    }

    busy_sleep( 1000 );

    finished_processes[ index ] = true;

    // Busy loop until all four periodic functions finish
    while( process_still_running( MAX_TEST_PROCESSES ) );

    OS_Signal( HOST_NOTIFY_SEM_SCHEDULING );
}

bool test_periodic_scheduling()
{
    int seed = get_time_us();
    print( "Seed %d\n", seed );
    srand( seed );

    init_periodic_params();

    memset8( (void*) finished_processes, 0x0, sizeof( finished_processes ) );
    OS_InitSem( HOST_NOTIFY_SEM_SCHEDULING, -MAX_TEST_PROCESSES + 1 );

    periodic_name_t process_names[MAXPROCESS];
    for( int i = 0; i < MAXPROCESS; i++ )
        process_names[ i ] = i;
    shuffle( process_names, sizeof( periodic_name_t ), MAXPROCESS );

    for( int i = 0; i < MAX_TEST_PROCESSES; i++ ) {
        PID pid = OS_Create( &periodic_test_func, i << 16 | process_names[ i ], PERIODIC, process_names[ i ] );
        ASSERT_NE( pid, INVALIDPID );
    }

    OS_Wait( HOST_NOTIFY_SEM_SCHEDULING );

    // Sleep for a full cycle to flush all the processes
    busy_sleep( cycle_len );
    return true;
}

static void sleep_proc_func()
{
    busy_sleep( 100 );
    int index = OS_GetParam();
    for( int i = index; i < MAX_TEST_PROCESSES; i++ )
        EXPECT_FALSE( finished_processes[ index ] );
    finished_processes[ index ] = true;
}

static void sleep_wait_func()
{
    while( process_still_running( MAX_TEST_PROCESSES ) )
        OS_Yield();
}

bool test_sporadic_scheduling()
{
    memset8( (void*) finished_processes, 0x0, sizeof( finished_processes ) );

    // Create a device function that repeatedly wakes up and does nothing
    // This is just to mess interrupt the scheduling
    PID pid_dev = OS_Create( &sleep_wait_func, 0, DEVICE, 1 );
    ASSERT_NE( pid_dev, INVALIDPID );

    for( int i = 0; i < MAX_TEST_PROCESSES; i++ ) {
        PID pid = OS_Create( &sleep_proc_func, i, SPORADIC, 0 );
        ASSERT_NE( pid, INVALIDPID );
    }

    OS_Yield();

    // Check all the functions run before us
    for( int i = 0; i < MAX_TEST_PROCESSES; i++ )
        ASSERT_TRUE( finished_processes[ i ] );

    // Sleep to make sure device function exits
    busy_sleep( 2 );

    return true;
}

void device_test_func()
{
    const int param = OS_GetParam();
    const int my_sleep = param & 0xffff;
    const int my_index = param >> 16;
    for( int i = 0; i < 1000 / my_sleep; i++ ) {
        const uint32_t next_wakeup = get_time_ms() + my_sleep;
        OS_Yield();
        EXPECT_GE( get_time_ms(), next_wakeup );
    }
    finished_processes[ my_index ] = true;
}

bool test_device_scheduling()
{
    int seed = get_time_us();
    print( "Seed %d\n", seed );
    srand( seed );

    memset8( (void*) finished_processes, 0x0, sizeof( finished_processes ) );

    for( int i = 0; i < MAX_TEST_PROCESSES; i++ ) {
        const int len = rand() % 1000 + 1;
        PID pid = OS_Create( &device_test_func, ( i << 16u ) | len, DEVICE, len );
        ASSERT_NE( pid, INVALIDPID );
    }

    // Wait for all processes to finish
    // Dont yield or block to make sure the devices are allowed to preempt us
    while( process_still_running( MAX_TEST_PROCESSES ) );

    return true;
}

static void empty_func()
{
}

static void blocked_func()
{
    OS_Wait( 0 );
    OS_Signal( HOST_NOTIFY_SEM_SCHEDULING );
}

bool test_invalid_process()
{
    int i = 0;
    while( OS_Create( &empty_func, 0, SPORADIC, 0 ) != INVALIDPID )
        i++;

    ASSERT_GE( i, MAX_TEST_PROCESSES );
    // There should be at least three process already running
    ASSERT_LE( i, MAXPROCESS - 3 );

    // Let all the processes run and exit
    OS_Yield();

    PID pid = OS_Create( NULL, 0, SPORADIC, 0 );
    ASSERT_EQ( pid, INVALIDPID );
    pid = OS_Create( &empty_func, 0, 3, 0 );
    ASSERT_EQ( pid, INVALIDPID );
    pid = OS_Create( &empty_func, 0, PERIODIC, MAXPROCESS );
    ASSERT_EQ( pid, INVALIDPID );

    OS_InitSem( 0, 0 );
    OS_InitSem( HOST_NOTIFY_SEM_SCHEDULING, 0 );

    // Create a period with name 1
    pid = OS_Create( &blocked_func, 0, PERIODIC, 1 );
    ASSERT_NE( pid, INVALIDPID );

    // Create a second period with name 1
    pid = OS_Create( &empty_func, 0, PERIODIC, 1 );
    ASSERT_EQ( pid, INVALIDPID );

    // Unblock the blocked function and wait for it to finish
    OS_Signal( 0 );
    OS_Wait( HOST_NOTIFY_SEM_SCHEDULING );

    return true;
}
