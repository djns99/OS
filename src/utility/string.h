#ifndef OS_STRING_H
#define OS_STRING_H
#include "types.h"

/**
 * Calculate the length of a zero terminated string
 * @param string The string length to calculate
 * @return The length of the string
 */
size_t strlen( const char* string );

/**
 * Copy a string
 * @param out The buffer to copy into
 * @param string The string to copy
 */
void strcpy( char* out, const char* string );

/**
 * Copy a string, limiting to a maximum number of bytes
 * @param out The buffer to copy into
 * @param string The string to copy
 * @param len The maximum length of the copied string, including the terminating 0
 */
void strncpy( char* out, const char* string, size_t len );

/**
 * Compare two strings
 * @param str1 The first string
 * @param str2 The second string
 * @return True if the strings are identical, false otherwise
 */
bool strcmp( const char* str1, const char* str2 );

#endif //OS_STRING_H
