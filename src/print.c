#include "types.h"
#include "print.h"
#include "string.h"

#define TEXT_MODE_WIDTH 80
#define TEXT_MODE_HEIGHT 25

typedef struct __attribute__((__packed__))
{
    uint8_t chr;
    uint8_t colour;
} video_mem_entry_t;

#define VIDEO_MEMORY ((video_mem_entry_t*) 0xb8000)

uint32_t row = 0;
uint32_t col = 0;
uint8_t curr_colour = 0x0f;

video_mem_entry_t* get_current_mem_offset()
{
    return VIDEO_MEMORY + row * TEXT_MODE_WIDTH + col;
}

void clear_screen() {
    video_mem_entry_t entry;
    entry.chr = '\0';
    entry.colour = curr_colour;
    os_memset16( VIDEO_MEMORY, *(uint16_t*)&entry, TEXT_MODE_WIDTH * TEXT_MODE_HEIGHT );
}

void write_char( uint8_t chr )
{
    video_mem_entry_t* entry = get_current_mem_offset();
    entry->chr = chr;
    entry->colour = 0x0f;
    
    row += col == (TEXT_MODE_WIDTH - 1);
    col = (col + 1) % TEXT_MODE_WIDTH;
}

void scroll()
{
    os_memcpy(VIDEO_MEMORY, VIDEO_MEMORY + TEXT_MODE_WIDTH, TEXT_MODE_WIDTH * sizeof(video_mem_entry_t));
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

void print( const char* msg )
{
    while( *msg )
    {
        if( *msg == '\r' )
            col = 0;
        else if ( *msg == '\n')
        {
            row++;
            col = 0;
        }
        else
            write_char( *msg );

        if ( row == TEXT_MODE_HEIGHT )
            scroll();
        
        msg++;
    }
}