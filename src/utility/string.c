#include "string.h"

uint32_t strlen( const char* string )
{
    uint32_t len = 0;
    while( *( string++ ) )
        len++;
    return len;
}

void strcpy( char* out, const char* string )
{
    while( *string )
        *( out++ ) = *( string++ );
    *out = '\0';
}

void strncpy( char* out, const char* string, size_t len )
{
    len--; // Subtract one to account for terminating char
    while( *string && len-- )
        *( out++ ) = *( string++ );
    *out = '\0';
}

bool strcmp( const char* str1, const char* str2 )
{
    while( *str1 && *str2 )
        if( *( str1++ ) != *( str2++ ) )
            return false;
    return !*str1 && !*str2;
}