#include <peripherals/keyboard.h>
#include <peripherals/timer.h>
#include <kernel.h>
#include <utility/print.h>
#include <utility/string.h>

extern void test_runner();

void print_uptime()
{
    char* unit = readline( "Enter Time Unit (us, ms, s, m, h) > " );
    if(!unit) {
        print("Failed to get time unit\n");
        return;
    }

    uint64_t time_us = get_time_us();
    if( strcmp(unit, "MS") )
        time_us /= 1e3;
    else if( strcmp(unit, "S") )
        time_us /= 1e6;
    else if( strcmp(unit, "M"))
        time_us /= 60e6;
    else if( strcmp(unit, "H") )
        time_us /= 3600e6;
    else if( !strcmp(unit, "US" ))
        print( "** Unrecognised time format, assuming us **\n" );

    print( "Current uptime is %U %s\n", time_us, unit );
    OS_Free( (MEMORY)unit );
}

void shell()
{
    while( true ) {
        char* line = readline( "Enter command > " );
        if( !line ) {
            print( "Failed to read line\n" );
            break;
        }

        if( strcmp( line, "TEST" ) )
            test_runner();
        else if( strcmp( line, "EXIT" ) )
            break;
        else if( strcmp( line, "UPTIME" ) )
            print_uptime();
        else if( strcmp( line, "HELP" ) )
            print( "Valid commands: TEST, HELP, UPTIME, EXIT\n" );
        else
            print( "Unrecognised command %s\nUse the HELP function to see available commands\n", line );

        OS_Free( (MEMORY) line );
    }
    print( "Shell Exited" );
}