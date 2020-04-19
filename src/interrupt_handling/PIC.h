#ifndef OS_PIC_H
#define OS_PIC_H
// Some magic from https://wiki.osdev.org/PIC

#include "utility/types.h"
#include "peripherals/io_ports.h"

#define PIC_MASTER_CONTROL (port_t)0x20
#define PIC_MASTER_DATA (port_t)(PIC_MASTER_CONTROL + 1)
#define PIC_SLAVE_CONTROL (port_t)0xA0
#define PIC_SLAVE_DATA (port_t)(PIC_SLAVE_CONTROL + 1)

static void remap_pic()
{
    const uint8_t old_data_master = port_read8( PIC_MASTER_DATA );
    const uint8_t old_data_slave = port_read8( PIC_SLAVE_DATA );

    port_write8( PIC_MASTER_CONTROL, 0x11 );
    port_write8( PIC_SLAVE_CONTROL, 0x11 );

    port_write8( PIC_MASTER_DATA, 0x20 );
    port_write8( PIC_SLAVE_DATA, 0x28 );

    port_write8( PIC_MASTER_DATA, 0x4 );
    port_write8( PIC_SLAVE_DATA, 0x2 );

    port_write8( PIC_MASTER_DATA, 0x1 );
    port_write8( PIC_SLAVE_DATA, 0x1 );

    port_write8( PIC_MASTER_DATA, old_data_master );
    port_write8( PIC_SLAVE_DATA, old_data_slave );
}

static void send_eoi( uint32_t irq_num )
{
    if( irq_num >= 8 )
        port_write8( PIC_SLAVE_CONTROL, 0x20 );
    port_write8( PIC_MASTER_CONTROL, 0x20 );
}

#endif //OS_PIC_H
