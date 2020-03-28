#ifndef IO_PORT_H
#define IO_PORT_H
#include "types.h"

extern void print_string_pm( const char* );

void print( const char* msg )
{
    print_string_pm( msg );
}

#endif