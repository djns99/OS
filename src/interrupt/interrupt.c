#include "interrupt.h"
#include "kernel.h"
#include "utility/types.h"

uint32_t num_disabled = 0;

void init_interrupts()
{
    num_disabled = 0;
    OS_EI();
}

void enable_interrupts()
{
    if( --num_disabled == 0)
            OS_EI();
}

void disable_interrupts()
{
    if( num_disabled++ == 0)
            OS_DI();
}