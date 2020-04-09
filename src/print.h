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

void set_fg_colour( uint8_t colour );

void set_bg_colour( uint8_t colour );

void clear_screen();

void print( const char* msg );

#endif