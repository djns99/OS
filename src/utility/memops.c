#include "memops.h"

void memcpy( void* dest, const void* src, size_t len )
{
    uint8_t* src8 = (uint8_t*) src;
    uint8_t* dest8 = (uint8_t*) dest;
    while( len-- )
        *( dest8++ ) = *( src8++ );
}

bool memcmp( const void* buf1, const void* buf2, size_t len )
{
    uint32_t* buf1_32 = (uint32_t*) buf1;
    uint32_t* buf2_32 = (uint32_t*) buf2;
    uint32_t i;
    for( i = 0; i < len / 4; i++ )
        if( buf1_32[ i ] != buf2_32[ i ] )
            return false;

    uint8_t* buf1_8 = (uint8_t*) buf1;
    uint8_t* buf2_8 = (uint8_t*) buf2;
    for( i *= 4; i < len; i++ )
        if( buf1_8[ i ] != buf2_8[ i ] )
            return false;

    return true;
}

void memset8( void* dest, uint8_t val, size_t len )
{
    uint32_t value = val;
    value |= value << 8;
    value |= value << 16;
    memset32( dest, value, ( len - len % 4 ) / 4 );

    uint8_t* dest8 = (uint8_t*) dest;
    for( uint32_t i = len - len % 4; i < len; i++ )
        dest8[ i ] = val;
}

void memset16( void* dest, uint16_t val, size_t len )
{
    uint32_t value = val;
    value |= value << 16;
    memset32( dest, value, ( len - len % 2 ) / 2 );
    uint16_t* dest16 = (uint16_t*) dest;
    for( uint32_t i = len - len % 2; i < len; i++ )
        dest16[ i ] = val;
}

void memset32( void* dest, uint32_t val, size_t len )
{
    uint32_t* dest32 = (uint32_t*) dest;
    while( len-- )
        *( dest32++ ) = val;
}

void memset64( void* dest, uint64_t val, size_t len )
{
    uint64_t* dest64 = (uint64_t*) dest;
    while( len-- )
        *( dest64++ ) = val;
}