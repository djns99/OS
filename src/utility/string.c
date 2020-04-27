#include "string.h"

uint32_t os_strlen( const char* string )
{
    uint32_t len = 0;
    while( *( string++ ) )
        len++;
    return len;
}

void os_strcpy( char* out, const char* string )
{
    while( *string )
        *( out++ ) = *( string++ );
}

void os_strncpy( char* out, const char* string, size_t len )
{
    while( *string && len-- )
        *( out++ ) = *( string++ );
}
