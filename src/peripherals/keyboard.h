#ifndef OS_KEYBOARD_H
#define OS_KEYBOARD_H

#include "utility/types.h"

/**
 * Initialise the keyboard subsystem
 */
void init_keyboard();

/**
 * Prompt the user for input and wait for them to type a line
 * @param prompt The prompt to present to the user
 * @return Allocated buffer containing the line read. The user is responsible for freeing this memory
 */
char* readline( const char* prompt );

#endif //OS_KEYBOARD_H
