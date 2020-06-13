#include "utility/memops.h"
#include "test_helper.h"
#include "test_suite.h"

#define HOST_NOTIFY_SEM (MAXSEM - 2)
bool awake[MAX_TEST_PROCESSES];

void sleep_func()
{
    uint32_t param = OS_GetParam();
    int semaphore_id = param & 0xFFFFu;
    uint32_t process_id = param >> 16u;
    OS_Wait( semaphore_id );
    awake[ process_id ] = true;
    OS_Signal( HOST_NOTIFY_SEM );
}

bool test_semaphore_mutex()
{
    memset8( awake, 0x0, sizeof( awake ) );
    OS_InitSem( 0, 1 );
    OS_InitSem( HOST_NOTIFY_SEM, 0 );

    PID pid1 = OS_Create( sleep_func, 0, SPORADIC, 0 );
    PID pid2 = OS_Create( sleep_func, 0x10000, SPORADIC, 0 );
    ASSERT_NE( pid1, INVALIDPID );
    ASSERT_NE( pid2, INVALIDPID );

    // Wait for one of the processes to signal us
    OS_Wait( HOST_NOTIFY_SEM );

    // Yield again to see if the other process will grab the semaphore
    OS_Yield();

    // Verify that the second process was never able to get the semaphore
    ASSERT_TRUE( awake[ 0 ] ^ awake[ 1 ] );

    // Signal the second process
    OS_Signal( 0 );

    // Wait for it to complete
    OS_Wait( HOST_NOTIFY_SEM );

    ASSERT_TRUE( awake[ 0 ] );
    ASSERT_TRUE( awake[ 1 ] );

    return true;
}

bool test_semaphore_n_blocked()
{
    memset8( awake, 0x0, sizeof( awake ) );

    const uint32_t num_processes = MAX_TEST_PROCESSES;
    OS_InitSem( 0, num_processes / 2 );
    OS_InitSem( HOST_NOTIFY_SEM, -( num_processes / 2 ) + 1 );

    for( uint32_t i = 0; i < num_processes; i++ ) {
        PID pid = OS_Create( sleep_func, i << 16u, SPORADIC, 0 );
        ASSERT_NE( pid, INVALIDPID );
    }

    // Wait for all the processes to signal us
    OS_Wait( HOST_NOTIFY_SEM );

    // Yield again to see if any remaining will grab the semaphore
    OS_Yield();

    for( uint32_t j = 0; j < num_processes / 2; j++ ) {
        // Verify that only half the processes woke up
        uint32_t num_awake = 0;
        for( uint32_t i = 0; i < num_processes; i++ ) {
            num_awake += awake[ i ];
        }
        ASSERT_EQ( num_awake, num_processes / 2 + j );

        if( num_awake < num_processes ) {
            // Signal the next process
            OS_Signal( 0 );

            // Wait for it to complete
            OS_Wait( HOST_NOTIFY_SEM );
        }
    }

    return true;
}