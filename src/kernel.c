#include "interrupt_handling/interrupt_handlers.h"
#include "print.h"
#include "kernel.h"
#include "timer.h"

void OS_Init()
{
    set_fg_colour( TEXT_GREEN );
    set_bg_colour( TEXT_BLACK );
    clear_screen();
    
    init_idt();
    init_timer( 1 );
    OS_InitMemory();
    
    OS_EI();
}

void OS_Start()
{
    // TODO Launch shell

    // TODO Sleep forever instead of busy loop
    uint32_t j = 0;
    asm("hlt");
}

void OS_Abort()
{
    print( "\nKernel abort triggered. Ceasing all operation\n" );
    // Disable interrupt and halts
    // Requires hard reboot to recover
    OS_DI();
    asm("hlt");
}

extern void init_memory( size_t kernel_start, size_t kernel_num_sectors );

int entry_point( size_t kernel_start, size_t kernel_num_sectors )
{
    init_memory( kernel_start, kernel_num_sectors );
    OS_Init();
    OS_Start();
}
