cmake_minimum_required(VERSION 3.13.3)
project(OS C ASM_NASM)
set(CMAKE_C_COMPILER /usr/local/i386elfgcc/bin/i386-elf-gcc)
set(CMAKE_C_LINK_EXECUTABLE "/usr/local/i386elfgcc/bin/i386-elf-ld <LINK_FLAGS> -o <TARGET> -Ttext 0x10000 <OBJECTS> --oformat binary")
set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -f elf -o <OBJECT> <SOURCE>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src/)

file(GLOB_RECURSE SRC_C_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE SRC_ASM_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm")
add_executable(kernel.bin kernel_entry.asm ${SRC_C_FILES} ${SRC_ASM_FILES})

add_custom_target(boot_sect_with_size.asm
        COMMAND ${CMAKE_SOURCE_DIR}/count_sectors.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/kernel.bin ${CMAKE_SOURCE_DIR}/boot_sect.asm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boot_sect_with_size.asm
        DEPENDS kernel.bin
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