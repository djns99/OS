cmake_minimum_required(VERSION 3.13.3)
project(OS C ASM_NASM)
set(CMAKE_C_COMPILER /usr/local/i386elfgcc/bin/i386-elf-gcc)
set(CMAKE_C_FLAGS "-ffreestanding ${CMAKE_C_FLAGS} -Werror -Wall -Os")
set(CMAKE_C_LINK_EXECUTABLE "/usr/local/i386elfgcc/bin/i386-elf-gcc <LINK_FLAGS> -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -o <TARGET> <OBJECTS> -ffreestanding -O2 -nostdlib -lgcc")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f elf -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -Werror -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
include_directories(src/)

file(GLOB_RECURSE SRC_C_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE SRC_ASM_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
file(GLOB_RECURSE PROGRAM_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/programs/*.c")

add_custom_target( linker DEPENDS linker.ld )

# Readable format
add_executable(kernel_readable.bin kernel_entry.asm ${SRC_ASM_FILES} ${SRC_C_FILES} ${PROGRAM_FILES})
add_dependencies(kernel_readable.bin linker)

# Binary format for actual image
add_custom_target(kernel.bin
        COMMAND objcopy -O binary ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel_readable.bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel.bin
        DEPENDS kernel_readable.bin
        BYPRODUCTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel.bin)

add_custom_target(boot_sect_with_size.asm
        COMMAND ${CMAKE_SOURCE_DIR}/count_sectors.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel_readable.bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel.bin ${CMAKE_SOURCE_DIR}/boot_sect.asm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect_with_size.asm
        DEPENDS kernel_readable.bin kernel.bin
        BYPRODUCTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect_with_size.asm)

add_custom_target(boot_sect.bin ALL
        nasm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect_with_size.asm -f bin -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect.bin
        DEPENDS boot_sect_with_size.asm
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        BYPRODUCTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect.bin)

add_custom_target(kernel-image.bin ALL
        cat ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect.bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel.bin > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel-image.bin
        DEPENDS boot_sect.bin kernel.bin
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        BYPRODUCTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel-image.bin)

add_custom_target(run
        qemu-system-i386 -fda ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel-image.bin
        DEPENDS kernel-image.bin)