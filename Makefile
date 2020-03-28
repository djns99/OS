all: kernel-image.bin



asm_files := $(wildcard 32bit/*.asm)
asm_files += kernel_entry.asm
obj_files := $(patsubst src/%.c,%.o,$(wildcard src/*.c))
%.o: src/%.c
	i386-elf-gcc -ffreestanding -c $< -o $@

%.o: 32bit/%.asm kernel_entry.asm
	nasm $< -f elf -o $@

kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@

boot_sect.bin: boot_sect.asm
	nasm $< -f bin -o $@

kernel.bin: $(asm_files) $(obj_files)
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel-image.bin: boot_sect.bin kernel.bin
	cat $^ > $@

run: all
	qemu-system-i386 -fda $<

.PHONY: test
test:
	make -C test/

clean:
	rm -f *.bin *.o *.dis