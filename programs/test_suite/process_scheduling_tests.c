#include "test_helper.h"
#include "test_suite.h"
#include "utility/print.h"
#define HOST_NOTIFY_SEM_SCHEDULING (MAXSEM - 3)

void running_a_bunch()
{
    print("ENTER THE GATES\n");
    int arg = OS_GetParam();
    for( int i = 0; i < 200000000; i += 2 ) {
        i--;
        if (i % 10000000 == 0) {
            print("%d: %d\n", arg,i);
        }
    }

    if (arg == 3) {
        OS_Create( (void ( * )( void )) running_a_bunch, 4, PERIODIC, 4);
    } else if (arg == 4) {
        OS_Create( (void ( * )( void )) running_a_bunch, 3, PERIODIC, 3);
    }
    OS_Signal(HOST_NOTIFY_SEM_SCHEDULING);
}

bool test_process_scheduling()
{
    OS_InitSem(HOST_NOTIFY_SEM_SCHEDULING, 1);
    for( int i = 0; i < 1; ++i ) {
        OS_Wait(HOST_NOTIFY_SEM_SCHEDULING);
    }
    // OS_Create( (void ( * )( void )) running_a_bunch, 1, DEVICE, 10);
    PID pid = OS_Create( (void ( * )( void )) running_a_bunch, 1, PERIODIC, 1);
    ASSERT_NE( pid, INVALIDPID );

    print("%d: %d\n", 888, 90999);
    OS_Yield();
    for( int i = 0; i < 2; ++i ) {
        OS_Wait(HOST_NOTIFY_SEM_SCHEDULING);
    }
    return true;
}
