#include <stdarg.h>
#include "types.h"
#include "print.h"
#include "memops.h"
#include "peripherals/io_ports.h"
#include "utility/debug.h"

#define TEXT_MODE_WIDTH 80u
#define TEXT_MODE_HEIGHT 25u

typedef struct __attribute__((__packed__)) {
    uint8_t chr;
    uint8_t colour;
} video_mem_entry_t;

extern video_mem_entry_t video_memory[];

size_t row = 0;
size_t col = 0;
uint8_t curr_colour = 0x0f;

size_t get_current_mem_offset()
{
    return row * TEXT_MODE_WIDTH + col;
}

video_mem_entry_t* get_current_mem_ptr()
{
    return video_memory + get_current_mem_offset();
}

void update_cursor()
{
    uint16_t pos = get_current_mem_offset();

    port_write16( 0x3D4, ( pos << 8u ) | 0x0Fu );
    port_write16( 0x3D4, ( pos & 0xFF00u ) | 0x0E );
}

void clear_range( uint32_t start, uint32_t num_chars )
{
    video_mem_entry_t entry;
    entry.chr = '\0';
    entry.colour = curr_colour;
    os_memset16( video_memory + start, *(uint16_t*) &entry, num_chars );
}

void scroll()
{
    if( row < TEXT_MODE_HEIGHT )
        return;
    const uint32_t memcpy_lines = (row - TEXT_MODE_HEIGHT) + 1;
    os_memcpy( video_memory, video_memory + TEXT_MODE_WIDTH * memcpy_lines,
               TEXT_MODE_WIDTH * ( TEXT_MODE_HEIGHT - memcpy_lines ) * sizeof( video_mem_entry_t ) );
    clear_range( TEXT_MODE_WIDTH * ( TEXT_MODE_HEIGHT - memcpy_lines ), memcpy_lines * TEXT_MODE_WIDTH );
    row = TEXT_MODE_HEIGHT - 1;
    col = 0;
}

void wrap_cursor()
{
    row += col / TEXT_MODE_WIDTH;
    col %= TEXT_MODE_WIDTH;

    if( row >= TEXT_MODE_HEIGHT ) {
        scroll();
    }
}

void clear_screen()
{
    video_mem_entry_t entry;
    entry.chr = '\0';
    entry.colour = curr_colour;
    os_memset16( video_memory, *(uint16_t*) &entry, TEXT_MODE_WIDTH * TEXT_MODE_HEIGHT );
    row = 0;
    col = 0;
    update_cursor();
}

void write_char_no_advance( uint8_t chr )
{
    video_mem_entry_t* entry = get_current_mem_ptr();
    entry->chr = chr;
    entry->colour = curr_colour;
}

void write_char( uint8_t chr )
{
    write_char_no_advance( chr );
    col++;
    wrap_cursor();
}

void set_fg_colour( uint8_t colour )
{
    curr_colour &= 0xF0u;
    curr_colour |= colour & 0xFu;
}

void set_bg_colour( uint8_t colour )
{
    curr_colour &= 0x0Fu;
    curr_colour |= colour << 4u;
}

void newline()
{
    row++;
    col = 0;
    if( row >= TEXT_MODE_HEIGHT ) {
        scroll();
    }
}

void carriage_return()
{
    col = 0;
}

void print_num_u( uint64_t val, uint32_t base )
{
    if( !val ) {
        write_char( '0' );
        return;
    }
    const char digits[16] = "0123456789abcdef";
    KERNEL_ASSERT( base != 0, "Illegal base 0" );
    KERNEL_ASSERT( base <= sizeof( digits ), "Base too large" );

    char converted_string[32];
    char* curr_char = converted_string;
    while( val ) {
        *curr_char++ = digits[ val % base ];
        val /= base;
    }

    while( curr_char != converted_string )
        write_char( *--curr_char );
}

void print_num_s( int64_t val, uint32_t base )
{
    if( val < 0 ) {
        val = -val;
        write_char( '-' );
    }

    print_num_u( val, base );
}

void handle_printf_arg( char chr, va_list* args );

void print_safe( const char* msg, bool use_format_string, va_list* args )
{
    while( *msg ) {
        if( *msg == '\r' ) {
            carriage_return();
        } else if( *msg == '\n' ) {
            newline();
        } else if( *msg == '\t' ) {
            col -= col % 4;
            col += 4;
            wrap_cursor();
        } else if( use_format_string && *msg == '%' ) {
            handle_printf_arg( *++msg, args );
        } else {
            write_char( *msg );
        }

        update_cursor();
        msg++;
    }
}

void handle_printf_arg( char chr, va_list* args )
{
    switch( chr ) {
        case 'p':
            print_safe( "0x", false, NULL );
            print_num_u( (uint32_t) va_arg( *args, void* ), 16 );
            return;
        case 'x':
            print_num_u( va_arg( *args, uint32_t ), 16 );
            return;
        case 'd':
            print_num_s( va_arg( *args, int32_t ), 10 );
            return;
        case 'u':
            print_num_u( va_arg( *args, uint32_t ), 10 );
            return;
        case 'b':
            print_num_u( va_arg( *args, uint32_t ), 2 );
            return;
        case 'o':
            print_num_u( va_arg( *args, uint32_t ), 8 );
            return;
        case 's':
            print_safe( va_arg( *args, const char* ), false, NULL );
            return;
        case 'c':
            write_char( (char) va_arg( *args, int ) );
            return;
        case 'U':
            print_num_u( va_arg( *args, uint64_t ), 10 );
            return;
        case 'D':
            print_num_u( va_arg( *args, int64_t ), 10 );
            return;
        case 'X':
            print_num_u( va_arg( *args, uint64_t ), 16 );
            return;
        case 'B':
            print_num_u( va_arg( *args, int64_t ), 2 );
            return;
        case 'O':
            print_num_u( va_arg( *args, uint64_t ), 8 );
            return;
        default:
            write_char( chr );
            return;
    }
}

void print( const char* msg, ... )
{
    va_list args;
    va_start( args, msg );
    print_safe( msg, true, &args );
    va_end( args );
}

void backspace()
{
    if( col > 0 ) {
        col--;
        write_char_no_advance( ' ' );
        update_cursor();
    }
}