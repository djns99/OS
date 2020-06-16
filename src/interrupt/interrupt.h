#ifndef OS_INTERRUPT_H
#define OS_INTERRUPT_H

/**
 * Initialises interrupts
 */
void init_interrupts();

/**
 * Enables interrupts if the interrupt refcount becomes zero
 */
void enable_interrupts();

/**
 * Increments the interrupt refcount and disables interrupts it was zero
 */
void disable_interrupts();

#endif //OS_INTERRUPT_H
