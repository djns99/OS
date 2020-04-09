#include "types.h"
#include "print.h"
#include "string.h"
#include "io_ports.h"

#define TEXT_MODE_WIDTH 80u
#define TEXT_MODE_HEIGHT 25u

typedef struct __attribute__((__packed__))
{
    uint8_t chr;
    uint8_t colour;
} video_mem_entry_t;

#define VIDEO_MEMORY ((video_mem_entry_t*) 0xb8000)

size_t row = 0;
size_t col = 0;
uint8_t curr_colour = 0x0f;

size_t get_current_mem_offset()
{
    return row * TEXT_MODE_WIDTH + col;
}

video_mem_entry_t* get_current_mem_ptr()
{
    return VIDEO_MEMORY + get_current_mem_offset();
}

void update_cursor() {
    uint16_t pos = get_current_mem_offset();

    port_write16(0x3D4, (pos << 8u) | 0x0Fu);
    port_write16(0x3D4, (pos & 0xFF00u) | 0x0E);
}

void clear_row( uint32_t row )
{
    video_mem_entry_t entry;
    entry.chr = '\0';
    entry.colour = curr_colour;
    os_memset16( VIDEO_MEMORY + row * TEXT_MODE_WIDTH, *(uint16_t*)&entry, TEXT_MODE_WIDTH );
}

void clear_screen() {
    video_mem_entry_t entry;
    entry.chr = '\0';
    entry.colour = curr_colour;
    os_memset16( VIDEO_MEMORY, *(uint16_t*)&entry, TEXT_MODE_WIDTH * TEXT_MODE_HEIGHT );
    row = 0;
    col = 0;
    update_cursor();
}

void scroll()
{
    while( row-- >= TEXT_MODE_HEIGHT)
    {
        os_memcpy(VIDEO_MEMORY, VIDEO_MEMORY + TEXT_MODE_WIDTH,
                  TEXT_MODE_WIDTH * (TEXT_MODE_HEIGHT - 1) * sizeof(video_mem_entry_t));
        clear_row( TEXT_MODE_HEIGHT - 1 );
    }
}

void write_char( uint8_t chr )
{
    video_mem_entry_t* entry = get_current_mem_ptr();
    entry->chr = chr;
    entry->colour = curr_colour;
    
    row += col == (TEXT_MODE_WIDTH - 1);
    col = (col + 1) % TEXT_MODE_WIDTH;

    if ( row >= TEXT_MODE_HEIGHT )
        scroll();
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
    if ( row >= TEXT_MODE_HEIGHT )
        scroll();
}

void carriage_return()
{
    col = 0;
}

void print( const char* msg )
{
    while( *msg )
    {
        if( *msg == '\r' )
            carriage_return();
        else if ( *msg == '\n')
            newline();
        else
            write_char( *msg );        

        update_cursor();
        msg++;
    }
}