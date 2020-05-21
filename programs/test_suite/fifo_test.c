#include "test_suite.h"
#include "test_helper.h"

void producer_func()
{
    FIFO f = OS_GetParam();
    OS_Write( f, 1 );
    OS_Yield();
    OS_Write( f, 2 );
    OS_Yield();
    OS_Write( f, 4 );
}

bool test_spsc_fifo()
{
    FIFO f = OS_InitFiFo();
    ASSERT_NE( f, INVALIDFIFO );

    PID pid = OS_Create( producer_func, f, SPORADIC, 0 );
    ASSERT_NE( pid, INVALIDPID );

    int res;
    while( !OS_Read( f, &res ) )
        OS_Yield();

    ASSERT_EQ( res, 1 );

    while( !OS_Read( f, &res ) )
        OS_Yield();

    ASSERT_EQ( res, 2 );

    while( !OS_Read( f, &res ) )
        OS_Yield();

    ASSERT_EQ( res, 4 );

    return true;
}

bool test_mpsc_fifo()
{
    ASSERT_LE( FIFOSIZE / 3, MAX_TEST_PROCESSES );
    const uint32_t num_procucers = FIFOSIZE / 3;
    FIFO f = OS_InitFiFo();
    ASSERT_NE( f, INVALIDFIFO );
    
    for( uint32_t i = 0; i < num_procucers; i++ ) {
        PID pid = OS_Create( producer_func, f, SPORADIC, 0 );
        ASSERT_NE( pid, INVALIDPID );
    }

    uint32_t cumulative = 0;
    uint32_t xor = 0;
    for( uint32_t i = 0; i < num_procucers * 3; i++ ) {
        int res;
        while( !OS_Read( f, &res ) )
            OS_Yield();

        cumulative += res;
        xor ^= res;
    }

    ASSERT_EQ( cumulative, num_procucers * 7 );
    ASSERT_EQ( xor, ( num_procucers % 2 ) * 0x7 );

    return true;
}

#define SUM_SEMAPHORE 0
#define HOST_SEMAPHORE 1
uint32_t cumulative;
uint32_t xor;

void consumer_func()
{
    FIFO f = OS_GetParam();
    
    int val;
    do {
        while( !OS_Read( f, &val ) )
            OS_Yield();
        OS_Wait( SUM_SEMAPHORE );
        cumulative += val;
        xor ^= val;
        OS_Signal( SUM_SEMAPHORE );
        OS_Signal( HOST_SEMAPHORE );
    } while ( val );
}

bool test_mpmc_fifo()
{
    cumulative = 0;
    xor = 0;
    
    ASSERT_LE( FIFOSIZE / 3, MAX_TEST_PROCESSES );
    const uint32_t num_procucers = FIFOSIZE / 3;
    const uint32_t num_consumers = MAX_TEST_PROCESSES - FIFOSIZE / 3;
    ASSERT_GE( num_consumers, 2 );
    ASSERT_LE( num_consumers, FIFOSIZE );
    FIFO f = OS_InitFiFo();
    ASSERT_NE( f, INVALIDFIFO );

    // Waits for all items to be read
    OS_InitSem( HOST_SEMAPHORE, 1-(num_procucers * 3) );
    // Mutex to protect globals
    OS_InitSem( SUM_SEMAPHORE, 1 );
    
    for( uint32_t i = 0; i < num_procucers; i++ ) {
        PID pid = OS_Create( producer_func, f, SPORADIC, 0 );
        ASSERT_NE( pid, INVALIDPID );
    }
    
    for( uint32_t i = 0; i < num_consumers; i++ ) {
        PID pid = OS_Create( consumer_func, f, SPORADIC, 0 );
        ASSERT_NE( pid, INVALIDPID );
    }
    
    OS_Wait( HOST_SEMAPHORE );
    
    for( uint32_t i = 0; i < num_consumers; i++ ) {
        // Flush consumers
        OS_Write( f, 0 );
    }
    
    ASSERT_EQ( cumulative, num_procucers * 7 );
    ASSERT_EQ( xor, ( num_procucers % 2 ) * 0x7 );

    return true;
}