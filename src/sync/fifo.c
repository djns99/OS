#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "fifo.h"
#include "kernel.h"
#include "utility/types.h"
#include "processes/syscall.h"

typedef struct {
    int32_t head;
    int32_t size;
    uint32_t buffer[FIFOSIZE];
} fifo_t;

fifo_t fifo_pool[MAXFIFO];

int init_fifo_syscall( uint32_t res, uint32_t _ )
{
    for( uint32_t i = 0; i < MAXFIFO; i++ ) {
        // Assume an empty fifo is free
        if( fifo_pool[ i ].size == 0 ) {
            fifo_pool[ i ].head = 0;
            fifo_pool[ i ].size = 0;
            *(FIFO*) res = i + 1;
            return SYS_SUCCESS;
        }
    }
    *(FIFO*) res = INVALIDFIFO;
    return SYS_SUCCESS;
}

int write_fifo_syscall( uint32_t param1, uint32_t param2 )
{
    FIFO f = param1;
    int val = param2;

    if( f == INVALIDFIFO || f > MAXFIFO )
        return SYS_INVLARG;

    disable_interrupts();

    fifo_t* fifo = &fifo_pool[ f - 1 ];
    if( fifo->head == -1 )
        return SYS_INVLARG;

    fifo->buffer[ fifo->head ] = val;
    fifo->head++;
    fifo->head %= FIFOSIZE;
    if( fifo->size < FIFOSIZE )
        fifo->size++;

    enable_interrupts();
    return 0;
}

int read_fifo_syscall( uint32_t param, uint32_t res )
{
    FIFO f = param;
    int* val = (int*) res;

    if( f == INVALIDFIFO || f > MAXFIFO || !val )
        return SYS_INVLARG;

    disable_interrupts();

    fifo_t* fifo = &fifo_pool[ f - 1 ];
    if( fifo->head == -1 )
        return SYS_INVLARG;

    if( !fifo->size ) {
        enable_interrupts();
        return SYS_FAILED;
    }

    const uint32_t idx = ( fifo->head - fifo->size + FIFOSIZE ) % FIFOSIZE;
    fifo->size--;
    *val = fifo->buffer[ idx ];

    enable_interrupts();
    return SYS_SUCCESS;
}

FIFO OS_InitFiFo()
{
    FIFO res;
    if( syscall( SYSCALL_FIFO_INIT, (uint32_t) &res, 0 ) != SYS_SUCCESS )
        return INVALIDFIFO;
    return res;
}

void OS_Write( FIFO f, int val )
{
    int res = syscall( SYSCALL_FIFO_WRITE, f, val );
    PROCESS_WARNING( res == SYS_SUCCESS, "Error writing to FIFO" );
}

BOOL OS_Read( FIFO f, int* val )
{
    int res = syscall( SYSCALL_FIFO_READ, f, (uint32_t) val );
    PROCESS_WARNING( res == SYS_SUCCESS || res == SYS_FAILED, "Error reading from FIFO" );
    return res == SYS_SUCCESS;
}

void init_fifos()
{
    for( uint32_t i = 0; i < MAXFIFO; i++ ) {
        fifo_pool[ i ].head = 0;
        fifo_pool[ i ].size = 0;
    }

    register_syscall_handler( SYSCALL_FIFO_INIT, &init_fifo_syscall );
    register_syscall_handler( SYSCALL_FIFO_WRITE, &write_fifo_syscall );
    register_syscall_handler( SYSCALL_FIFO_READ, &read_fifo_syscall );
}
