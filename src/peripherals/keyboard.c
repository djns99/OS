#include "interrupt/interrupt_handlers.h"
#include "keyboard.h"
#include "io_ports.h"
#include "utility/print.h"

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

void keyboard_handler( interrupt_params_t* r )
{
    uint8_t scancode = port_read8( 0x60 );

    if( scancode > SC_MAX )
        return;

    if( scancode == BACKSPACE ) {
        backspace();
    } else if( scancode == ENTER ) {
        print( "\n" );
    } else {
        char letter = sc_ascii[ (int) scancode ];
        /* Remember that kprint only accepts char[] */
        char str[2] = { letter, '\0' };
        print( str );
    }
}

void init_keyboard()
{
    register_handler( IRQ1, keyboard_handler );
}