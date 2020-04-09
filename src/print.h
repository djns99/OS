#ifndef IO_PORT_H
#define IO_PORT_H
#include "types.h"

// FG & BG
#define FG_BLACK 0x0
#define FG_WHITE 0xf
#define FG_L_GREY 0x7
#define FG_D_GREY 0x8
#define FG_BLUE 0x1
#define FG_GREEN 0x2
#define FG_CYAN 0x3
#define FG_RED 0x4
#define FG_MAGENTA 0x5
#define FG_BROWN 0x6
#define FG_L_BLUE 0x9
#define FG_L_GREEN 0xa
#define FG_L_CYAN 0xb
#define FG_L_RED 0xc
#define FG_L_MAGENTA 0xd
#define FG_YELLOW 0xe

void set_fg_colour( uint8_t colour );
void set_bg_colour( uint8_t colour );
void clear_screen();
void print( const char* msg );

#endif