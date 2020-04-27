#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "fifo.h"
#include "kernel.h"
#include "utility/types.h"

typedef struct {
    int32_t head;
    int32_t size;
    uint32_t buffer[FIFOSIZE];
} fifo_t;

fifo_t fifo_pool[MAXFIFO];

void init_fifos()
{
    for( uint32_t i = 0; i < MAXFIFO; i++ ) {
        fifo_pool[ i ].head = -1;
        fifo_pool[ i ].size = -1;
    }
}

FIFO OS_InitFiFo()
{
    for( uint32_t i = 0; i < MAXFIFO; i++ ) {
        if( fifo_pool[ i ].head != NULL ) {
            fifo_pool[ i ].head = 0;
            fifo_pool[ i ].size = 0;
            return i + 1;
        }
    }
    return INVALIDFIFO;
}

void OS_Write( FIFO f, int val )
{

    KERNEL_ASSERT( f != INVALIDFIFO, "Tried to write to invalid FIFO" );

    disable_interrupts();

    fifo_t* fifo = &fifo_pool[ f - 1 ];
    KERNEL_ASSERT( fifo->head != -1, "Tried to write to invalid FIFO" );

    fifo->buffer[ fifo->head ] = val;
    fifo->head++;
    fifo->head %= FIFOSIZE;
    if( fifo->size < FIFOSIZE )
        fifo->size++;


    enable_interrupts();
}

BOOL OS_Read( FIFO f, int* val )
{
    KERNEL_ASSERT( f != INVALIDFIFO, "Tried to write to invalid FIFO" );

    disable_interrupts();

    fifo_t* fifo = &fifo_pool[ f - 1 ];
    KERNEL_ASSERT( fifo->head != -1, "Tried to write to invalid FIFO" );

    if( !fifo->size ) {
        enable_interrupts();
        return false;
    }

    const uint32_t idx = ( fifo->head - fifo->size + FIFOSIZE ) % FIFOSIZE;
    fifo->size--;
    *val = fifo->buffer[ idx ];

    enable_interrupts();
    return true;
}
