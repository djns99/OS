#include "print.h"

int main()
{
    set_fg_colour( FG_D_GREY );
    set_bg_colour( FG_L_GREY );
//    clear_screen();
    print("Hello World");
    while (true);
}
