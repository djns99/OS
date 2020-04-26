target remote 127.0.0.1:1234
layout src
#layout asm
layout regs
tui reg all
file cmake-build-debug/bin/kernel_readable.bin
si
#break *0x10000
focus cmd
break entry_point
break OS_Abort
break isr_handler
#break fork_process
continue
