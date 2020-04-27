#include "memops.h"

void os_memcpy( void* dest, const void* src, size_t len )
{
    uint8_t* src8 = (uint8_t*) src;
    uint8_t* dest8 = (uint8_t*) dest;
    while( len-- )
        *( dest8++ ) = *( src8++ );
}

bool os_memcmp( const void* buf1, const void* buf2, size_t len ) {
    uint32_t* buf1_32 = (uint32_t*) buf1;
    uint32_t* buf2_32 = (uint32_t*) buf2;
    uint32_t i;
    for( i = 0; i <= len - 4; buf1_32++, buf2_32++, i += 4 )
        if( *buf1_32 != *buf2_32 )
            return false;

    uint8_t* buf1_8 = (uint8_t*) buf1_32;
    uint8_t* buf2_8 = (uint8_t*) buf2_32;
    for( ; i < len; buf1_8++, buf2_8++, i++)
        if( *buf1_8 != *buf2_8 )
            return false;
    
    return true;
}

void os_memset8( void* dest, uint8_t val, size_t len )
{
    uint32_t value = val;
    value |= value << 8;
    value |= value << 16;
    os_memset32( dest, value, (len - len % 4) / 4 );

    uint8_t* dest8 = (uint8_t*) dest;
    for( uint32_t i = len - len % 4; i < len; i++ )
        dest8[i] = val;
}

void os_memset16( void* dest, uint16_t val, size_t len )
{
    uint32_t value = val;
    value |= value << 16;
    os_memset32( dest, value, (len - len % 2) / 2 );
    uint16_t* dest16 = (uint16_t*) dest;
    for( uint32_t i = len - len % 2; i < len; i++ )
        dest16[i] = val;
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