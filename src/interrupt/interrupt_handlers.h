#ifndef OS_INTERRUPT_HANDLERS_H
#define OS_INTERRUPT_HANDLERS_H
#include "utility/types.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) interrupt_params_t;

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

#define SYSCALL_IRQ 0x80

void init_idt();

typedef void(* irq_handler_t)( interrupt_params_t* );
void register_handler( uint32_t irq_num, irq_handler_t handler );

#endif //OS_INTERRUPT_HANDLERS_H
