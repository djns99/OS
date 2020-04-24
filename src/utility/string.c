#include "string.h"

void os_memcpy( void* dest, const void* src, size_t len )
{
    uint8_t* src8 = (uint8_t*) src;
    uint8_t* dest8 = (uint8_t*) dest;
    while( len-- )
        *( dest8++ ) = *( src8++ );
}

void os_memset8( void* dest, uint8_t val, size_t len )
{
    uint8_t* dest8 = (uint8_t*) dest;
    while( len-- )
        *( dest8++ ) = val;
}

void os_memset16( void* dest, uint16_t val, size_t len )
{
    uint16_t* dest16 = (uint16_t*) dest;
    while( len-- )
        *( dest16++ ) = val;
}

void os_memset32( void* dest, uint32_t val, size_t len )
{
    uint32_t* dest32 = (uint32_t*) dest;
    while( len-- )
        *( dest32++ ) = val;
}

void os_memset64( void* dest, uint64_t val, size_t len )
{
    uint64_t* dest64 = (uint64_t*) dest;
    while( len-- )
        *( dest64++ ) = val;
}

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
