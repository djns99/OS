#ifndef IO_PORT_H
#define IO_PORT_H
#include "types.h"

// FG & BG
#define TEXT_BLACK 0x0
#define TEXT_WHITE 0xf
#define TEXT_L_GREY 0x7
#define TEXT_D_GREY 0x8
#define TEXT_BLUE 0x1
#define TEXT_GREEN 0x2
#define TEXT_CYAN 0x3
#define TEXT_RED 0x4
#define TEXT_MAGENTA 0x5
#define TEXT_BROWN 0x6
#define TEXT_L_BLUE 0x9
#define TEXT_L_GREEN 0xa
#define TEXT_L_CYAN 0xb
#define TEXT_L_RED 0xc
#define TEXT_L_MAGENTA 0xd
#define TEXT_YELLOW 0xe

/**
 * Set the text colour for the console
 * @param colour The colour to set to
 */
void set_fg_colour( uint8_t colour );
/**
 * Set the background colour for the console
 * @param colour The colour to set to
 */
void set_bg_colour( uint8_t colour );

/**
 * Gets the current text colour of the console
 * @return The current text colour of the console
 */
uint8_t get_fg_colour();
/**
 * Gets the current background colour of the console
 * @return The current background colour of the console
 */
uint8_t get_bg_colour();

/**
 * Clear the screen
 */
void clear_screen();

/**
 * Print a string to the console
 * @param msg The format string
 * @param ... Format values
 *
 * Format specifiers:
 * p: Print a pointer
 * x: Print a hexadecimal uint32_t
 * d: Print a decimal int32_t
 * u: Print a decimal uint32_t
 * b: Print a binary uint32_t
 * o: Print an octal uint32_t
 * X: Print a hexadecimal uint64_t
 * D: Print a decimal int64_t
 * U: Print a decimal uint64_t
 * B: Print a binary uint64_t
 * O: Print an octal uint64_t
 * s: Print a zero terminated string
 * c: Print a character
 */
void print( const char* msg, ... );

/**
 * Backspace one character on the current line
 */
void backspace();

#endif