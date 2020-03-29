#ifndef IO_PORT_H
#define IO_PORT_H
#include "types.h"

// TODO Figure out IO ports
//typedef uint16_t io_port_t;
//
//#define GRAPHICS_BASE (io_port_t)0x3D5
//
//#define TEXT_CURSOR_LOCATION_LOW_PORT GRAPHICS_BASE + 0x0F
//#define SCREEN_START_ADDRESS_LOW_PORT GRAPHICS_BASE + 0x0D
//#define HORIZONTAL_TOTAL_PORT GRAPHICS_BASE + 0x00
//
//void write_to_port( io_port_t port, io_port_t value )
//{
//    asm volatile( "out %1, %0" : : "Nd" (value), "a" (port) );
//}


void print( const char* msg )
{
    uint16_t* video_memory = (uint16_t*) 0xb8000;

    while( *msg )
    {
        *video_memory = 0x0f00 | *msg;
        video_memory++;
        msg++;
    }
}

#endif