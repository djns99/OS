#ifndef IO_PORT_H
#define IO_PORT_H
#include "types.h"

void set_fg_colour( uint8_t colour );
void set_bg_colour( uint8_t colour );
void print( const char* msg );

#endif