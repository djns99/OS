#include "string.h"

void os_memcpy( void* dest, const void* src, uint32_t len )
{
    uint8_t* src8 = (uint8_t*)src;
    uint8_t* dest8 = (uint8_t*)dest;
    while( len-- )
        *(dest8++) = *(src8++);
}

void os_memset( void* dest, uint8_t val, uint32_t len )
{
    uint8_t* dest8 = (uint8_t*)dest;
    while( len-- )
        *(dest8++) = val;
}

uint32_t os_strlen( const char* string )
{
    uint32_t len = 0;
    while( *(string++) )
        len++;
    return len;
}

void os_strcpy( char* out, const char* string )
{
    while( *string )
        *(out++) = *(string++);
}

void os_strncpy( char* out, const char* string, uint32_t len )
{
    while( *string && len-- )
        *(out++) = *(string++);
}
