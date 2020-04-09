#include "memory.h"
#include "kernel.h"

page_table_entry_t* global_page_directory;

void OS_InitMemory();
MEMORY OS_Malloc( int val );
BOOL OS_Free( MEMORY m );

