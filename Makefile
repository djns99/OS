
all: kernel-image.bin

src_files := $(wildcard src/*.c)
obj_files := $(patsubst src/%.c,%.o,$(src_files))

# Stuff is broke - always recompile everything
.PHONY: $(src_files)
%.o: src/%.c
	i386-elf-gcc -ffreestanding -c $< -o $@

kernel_entry.o: kernel_entry.asm
	nasm $< -f elf -o $@

kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@

boot_sect.bin: boot_sect.asm
	nasm $< -f bin -o $@

kernel.bin: $(obj_files) kernel_entry.o
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel-image.bin: boot_sect.bin kernel.bin
	cat $^ > $@

run: all
	qemu-system-i386 -fda $<

.PHONY: test
test:
	make -C test/

.PHONY: clean
clean:
	rm -f *.bin *.o *.dis