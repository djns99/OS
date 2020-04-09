#include "print.h"

int main()
{
    set_fg_colour( TEXT_D_GREY );
    set_bg_colour( TEXT_YELLOW );
    clear_screen();
    print("Hello World");
}
