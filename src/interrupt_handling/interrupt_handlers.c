#include "debug.h"
#include "string.h"
#include "print.h"
#include "types.h"
#include "interrupt_handlers.h"
#include "PIC.h"

// Handlers in ASM file
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

typedef struct {
    uint16_t low_ptr;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t high_ptr;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
typedef idt_entry_t idt_t[IDT_ENTRIES];
idt_t idt;
idt_register_t idt_reg;
irq_handler_t irq_handlers[IDT_ENTRIES];

void register_internal_handler( uint32_t irq_num, void(* function)( void ) )
{
    KERNEL_ASSERT( function, "NULL Interrupt handler provided" );

    size_t f_ptr = (size_t) function;
    idt[ irq_num ].low_ptr = (uint16_t) (f_ptr & 0xFFFF);
    idt[ irq_num ].high_ptr = (uint16_t) (( f_ptr >> 16 ) & 0xFFFF);
    idt[ irq_num ].selector = 0x08;
    idt[ irq_num ].type_attr = 0x8E;
    idt[ irq_num ].zero = 0x0;
}

void init_idt()
{
    const typeof( &isr0 ) isr_funcs[32] = { &isr0, &isr1, &isr2, &isr3, &isr4, &isr5, &isr6, &isr7, &isr8, &isr9, &isr10,
                                            &isr11, &isr12, &isr13, &isr14, &isr15, &isr16, &isr17, &isr18, &isr19, &isr20,
                                            &isr21, &isr22, &isr23, &isr24, &isr25, &isr26, &isr27, &isr28, &isr29, &isr30,
                                            &isr31, };

    const typeof( &irq0 ) irq_funcs[16] = { &irq0, &irq1, &irq2, &irq3, &irq4, &irq5, &irq6, &irq7, &irq8, &irq9, &irq10,
                                            &irq11, &irq12, &irq13, &irq14, &irq15, };
    
    os_memset( idt, 0x0, sizeof( idt ) );
    os_memset( irq_handlers, 0x0, sizeof( irq_handlers ) );

    const uint32_t num_isr = sizeof( isr_funcs ) / sizeof( isr_funcs[ 0 ] );
    for( uint32_t i = 0; i < num_isr; i++ ) {
        register_internal_handler( i, isr_funcs[ i ] );
    }

    remap_pic();

    for( uint32_t i = 0; i < sizeof( irq_funcs ) / sizeof( irq_funcs[ 0 ] ); i++ ) {
        register_internal_handler( num_isr + i, irq_funcs[ i ] );
    }

    idt_reg.limit = sizeof( idt ) - 1;
    idt_reg.base = (uint32_t) &idt;
    asm("lidt (%0)"::"r" (&idt_reg));

    print( "Successfully initialised the IDT\n" );
}

void register_handler( uint32_t irq_num, irq_handler_t handler )
{
    irq_handlers[ irq_num ] = handler;
}

void isr_handler( registers_t r )
{
    /* To print the message which defines every exception */
    const char* exception_messages[] = { "Division By Zero", "Debug", "Non Maskable Interrupt", "Breakpoint",
                                   "Into Detected Overflow", "Out of Bounds", "Invalid Opcode", "No Coprocessor",

                                   "Double Fault", "Coprocessor Segment Overrun", "Bad TSS", "Segment Not Present",
                                   "Stack Fault", "General Protection Fault", "Page Fault", "Unknown Interrupt",
                                   "Coprocessor Fault", "Alignment Check", "Machine Check",

                                   "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
                                   "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved" };
    
    print( "Received interrupt: %d (%s)\n", r.int_no, exception_messages[ r.int_no ] );
}

void irq_handler( registers_t r )
{
    print( "Received irq: %d\n", r.int_no );
    send_eoi( r.int_no );

    if( irq_handlers[ r.int_no ] != NULL )
        irq_handlers[ r.int_no ]( r );
}