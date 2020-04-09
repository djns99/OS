#include "io.h"

int main()
{
    asm("jmp 2");
    asm("jmp 2");
    asm("jmp -1");
    asm("jmp -3");
    print("Hello World");
    while(true);
    return 0;
}
