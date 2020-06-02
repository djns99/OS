#include "test_helper.h"
#include "test_suite.h"
#include "utility/print.h"
#define HOST_NOTIFY_SEM_SCHEDULING (MAXSEM - 3)

void running_a_bunch()
{
    int arg = OS_GetParam();

    if (arg == 1)
        OS_Create( (void ( * )( void )) running_a_bunch, 2, PERIODIC, 2);
    else if(arg == 2)
        OS_Create( (void ( * )( void )) running_a_bunch, 3, PERIODIC, 3);
    else if(arg == 3)
        OS_Create( (void ( * )( void )) running_a_bunch, 4, PERIODIC, 4);
    OS_Signal(HOST_NOTIFY_SEM_SCHEDULING);
}

bool test_process_scheduling()
{
    OS_InitSem(HOST_NOTIFY_SEM_SCHEDULING, -3);
    PID pid = OS_Create( (void ( * )( void )) running_a_bunch, 1, PERIODIC, 1);
    ASSERT_NE( pid, INVALIDPID );
    
    OS_Yield();
    OS_Wait(HOST_NOTIFY_SEM_SCHEDULING);
    return true;
}
