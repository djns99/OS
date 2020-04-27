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
    const uint32_t num_procucers = FIFOSIZE / 3;
    FIFO f = OS_InitFiFo();

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
