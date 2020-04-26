#include "utility/debug.h"
#include "interrupt.h"
#include "kernel.h"
#include "utility/types.h"
#include "processes/process.h"

void init_interrupts()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables == 1, "Idle process incorrectly configured interrupts" );
}

void enable_interrupts()
{
    KERNEL_ASSERT( get_current_process()->interrupt_disables > 0,
                   "Process tried to enable interrupts without corresponding disable" );
    if( --get_current_process()->interrupt_disables == 0 )
            OS_EI();
}

void disable_interrupts()
{
    if( get_current_process()->interrupt_disables++ == 0 )
            OS_DI();
}