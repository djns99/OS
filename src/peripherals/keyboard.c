#include "utility/memops.h"
#include "processes/process.h"
#include "memory/memory.h"
#include "interrupt/interrupt.h"
#include "utility/debug.h"
#include "interrupt/interrupt_handlers.h"
#include "keyboard.h"
#include "io_ports.h"
#include "utility/print.h"
#include "processes/syscall.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C

#define SC_MAX 57
const char* sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace",
                          "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl", "A", "S",
                          "D", "F", "G", "H", "J", "K", "L", ";", "'", "`", "LShift", "\\", "Z", "X", "C", "V", "B",
                          "N", "M", ",", ".", "/", "RShift", "Keypad *", "LAlt", "Spacebar" };
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E',
                          'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K',
                          'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '?', '?',
                          '?', ' ' };

char* current_buffer = NULL;
size_t buffer_len;
int buffer_pos;

list_head_t blocked_processes;
list_head_t reading_process;

void unblock_waiting_reader()
{
    print("\n");
    disable_interrupts();
    if( current_buffer ) {
        KERNEL_ASSERT( buffer_pos <= buffer_len, "Overran read line buffer" );
        // Unblock the waiting process
        schedule_blocked( &reading_process );
    }
    enable_interrupts();
}

bool extend_buffer()
{
    if( buffer_pos >= buffer_len || !current_buffer ) {
        if( !buffer_len )
            buffer_len = 256;
        else
            buffer_len *= 2;

        char* new_buf = (char*) kmalloc( buffer_len );
        if( !new_buf )
            return false;

        if( current_buffer )
            kfree( current_buffer );
        current_buffer = new_buf;
    }
    return true;
}

void add_char_to_line( char c )
{
    disable_interrupts();
    if( !extend_buffer() ) {
        unblock_waiting_reader();
    } else {
        current_buffer[ buffer_pos++ ] = c;
        /* Remember that kprint only accepts char[] */
        char str[2] = { c, '\0' };
        print( str );
    }
    enable_interrupts();
}

void remove_char_from_line()
{
    disable_interrupts();
    if( buffer_pos ) {
        buffer_pos--;
        backspace();
    }
    enable_interrupts();
}

void keyboard_handler( interrupt_params_t* r )
{
    uint8_t scancode = port_read8( 0x60 );

    if( scancode > SC_MAX )
        return;

    if( scancode == BACKSPACE ) {
        remove_char_from_line();
    } else if( scancode == ENTER ) {
        unblock_waiting_reader();
    } else {
        char letter = sc_ascii[ (int) scancode ];
        add_char_to_line( letter );
    }
}

int readline_syscall( uint32_t param1, uint32_t param2 )
{
    // Set ourselves as the current reader
    disable_interrupts();
    while( current_buffer ) {
        block_process( &blocked_processes, get_current_process() );
    }
    buffer_pos = 0;
    buffer_len = 0;
    if( !extend_buffer() ) {
        enable_interrupts();
        // Failed to allocate
        return SYS_FAILED;
    }
    enable_interrupts();

    print( "%s", (const char*) param1 );
    // Block and wait for a line to be read
    block_process( &reading_process, get_current_process() );

    int res = SYS_FAILED;
    char* user_data = user_malloc( buffer_pos + 1 );
    if( user_data ) {
        char** line = (char**) param2;
        *line = user_data;
        memcpy( user_data, current_buffer, buffer_pos + 1 );
        user_data[ buffer_pos ] = '\0';
        res = SYS_SUCCESS;
    }

    // Release the keyboard for a different process
    disable_interrupts();
    current_buffer = NULL;
    schedule_blocked( &blocked_processes );
    enable_interrupts();

    return res;
}

char* readline( const char* prompt )
{
    char* line;
    int res = syscall( SYSCALL_READ_LINE, (uint32_t) prompt, (uint32_t) &line );
    if( res == SYS_FAILED )
        return NULL;
    KERNEL_WARNING( res == SYS_SUCCESS, "Error reading line" );
    return line;
}

void init_keyboard()
{
    register_handler( IRQ1, keyboard_handler );
    register_syscall_handler( SYSCALL_READ_LINE, &readline_syscall );
    init_list( &blocked_processes );
    init_list( &reading_process );
}
