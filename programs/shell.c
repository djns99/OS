#include <peripherals/keyboard.h>
#include <kernel.h>
#include <utility/print.h>
#include <utility/string.h>

extern void test_runner();

void shell() {
    while (true) {
        char* line = readline( "Enter command > " );
        if( !line ) {
            print( "Failed to read line\n" );
            break;
        }

        if( strcmp( line, "TEST" ) )
            test_runner();
        else if( strcmp( line, "EXIT" ) )
            break;
        else if( strcmp( line, "HELP" ))
            print( "Valid commands: TEST, HELP, EXIT\n" );
        else
            print( "Unrecognised command %s\nUse the HELP function to see available commands\n", line );
    }
    print("Shell Exiting");
}