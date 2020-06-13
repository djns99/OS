target remote 127.0.0.1:1234
layout src
layout asm
layout split
file cmake-build-debug/bin/kernel_readable.bin
si
#break *0x10000
focus cmd
break entry_point
break OS_Abort
break isr_handler
break debug_kern_warn_break
break interrupt_handlers.c:145
#break fork_process
continue
