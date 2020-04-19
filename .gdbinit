target remote 127.0.0.1:1234
layout regs
tui reg all
file cmake-build-debug/bin/kernel_readable.bin
si
#break *0x10000
break entry_point
continue
