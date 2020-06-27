#include "utility/rand.h"
#include "peripherals/timer.h"
#include "test_helper.h"
#include "test_suite.h"
#include "utility/print.h"
#include "utility/memops.h"
#include "utility/types.h"

#define HOST_NOTIFY_SEM_SCHEDULING (MAXSEM - 3)

static void sleep( uint64_t sleep_ms )
{
    uint64_t target_time = get_time_ms() + sleep_ms;
    while( get_time_ms() < target_time );
}

volatile bool finished_periodics[4];

void periodic_test_func()
{
    int arg = OS_GetParam();

    if( arg == 0 )
        OS_Create( &periodic_test_func, 1, PERIODIC, 2 );
    else if( arg == 1 )
        OS_Create( &periodic_test_func, 2, PERIODIC, 3 );
    else if( arg == 2 )
        OS_Create( &periodic_test_func, 3, PERIODIC, 4 );

    sleep( 30 );
    finished_periodics[ arg ] = true;
    // Busy loop until all four periodic functions finish
    while( !( finished_periodics[ 0 ] & finished_periodics[ 1 ] & finished_periodics[ 2 ] & finished_periodics[ 3 ] ) );

    OS_Signal( HOST_NOTIFY_SEM_SCHEDULING );
}

bool test_periodic_scheduling()
{
    memset8( (void*) finished_periodics, 0x0, sizeof( finished_periodics ) );
    OS_InitSem( HOST_NOTIFY_SEM_SCHEDULING, -3 );
    PID pid = OS_Create( &periodic_test_func, 0, PERIODIC, 1 );
    ASSERT_NE( pid, INVALIDPID );

    OS_Yield();
    OS_Wait( HOST_NOTIFY_SEM_SCHEDULING );
    return true;
}

volatile bool sleep_finished = false;

static void sleep_proc_func()
{
    sleep( OS_GetParam() );
    sleep_finished = true;
}

static void sleep_wait_func()
{
    while( !sleep_finished )
        OS_Yield();
}

bool test_sporadic_scheduling()
{
    sleep_finished = false;

    PID pid = OS_Create( &sleep_proc_func, 1000, SPORADIC, 0 );
    ASSERT_NE( pid, INVALIDPID );

    // Create a device function that repeatedly wakes up and does nothing
    // This is just to mess interrupt the scheduling
    PID pid_dev = OS_Create( &sleep_wait_func, 0, DEVICE, 1 );
    ASSERT_NE( pid_dev, INVALIDPID );

    OS_Yield();

    // Check that we were not scheduled over top of the function
    ASSERT_TRUE( sleep_finished );

    // Sleep to make sure device function exits
    sleep( 2 );

    return true;
}

void device_test_func()
{
    const int my_sleep = OS_GetParam();
    for( int i = 0; i < 1000 / my_sleep; i++ ) {
        const uint32_t next_wakeup = get_time_ms() + my_sleep;
        OS_Yield();
        EXPECT_GE( get_time_ms(), next_wakeup );
    }
    OS_Signal( HOST_NOTIFY_SEM_SCHEDULING );
}

bool test_device_scheduling()
{
    srand( get_time_us() );
    OS_InitSem( HOST_NOTIFY_SEM_SCHEDULING, -MAX_TEST_PROCESSES + 1 );
    for( int i = 0; i < MAX_TEST_PROCESSES; i++ ) {
        const int len = rand() % 1000 + 1;
        PID pid = OS_Create( &device_test_func, len, DEVICE, len );
        ASSERT_NE( pid, INVALIDPID );
    }

    OS_Wait( HOST_NOTIFY_SEM_SCHEDULING );
    return true;
}

bool test_invalid_process()
{
    print("Test start\n");
    int i = 0;
    while( OS_Create( &sleep_proc_func, 0, SPORADIC, 0 ) != INVALIDPID )
        i++;

    ASSERT_GE( i, MAX_TEST_PROCESSES );
    // There should be at least three process already running
    ASSERT_LE( i, MAXPROCESS - 3 );

    // Let all the processes run and exit
    OS_Yield();

    return true;
}
