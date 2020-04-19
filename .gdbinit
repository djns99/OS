target remote 127.0.0.1:1234
layout regs
tui reg all
si
break *0x10000
continue
