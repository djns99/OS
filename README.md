# OS

The src/ folder contains all the C code for the operating system, while the boot/ contains the assembly files used for booting.
The programs/ directory contains 'user' applications that use the OS.
Finally boot_sect.asm and kernel_entry.asm provide the initial entry points for booting and the kernel respectively

## Building

### Dependencies

This library uses `cmake` and `make` for the build process.
By default it requires `i386-elf-gcc` gcc library to compile, though normal `gcc` can be used with minor modifications to the `CMakeLists.txt`
In order to run the OS, `qemu` is recommended, and the build system has integrated support for running with `qemu-system-i386`
To debug the project has facilities for debugging with `gdb`

### Compiling

To compile the kernel run the following commands:

```bash
mkdir build && cd build
cmake ..
make all
```

### Running

Either run `make run` from the `build` directory you created in the previous step for a quick run
or use the `qemu-launch.sh` script for more options.

The `qemu-launch.sh` script usage is as follows:

```bash
qemu-launch.sh             # No params to simply run the command
qemu-launch.sh monitor     # To launch a monitor server at localhost:1235
                           # This can be accessed with `telnet 127.0.0.1 1235`
qemu-launch.sh gdb         # To launch and wait for a gdb connection
                           # For convenience a `.gdbinit` file is provided
                           # This file will automatically connect
                           # and set some useful debugging breakpoints
                           # NOTE: It may be necessary to change the path to the compiled kernel
qemu-launch.sh <args>      # Forwards the args to qemu
```

## Known Bugs

There is currently a few known bugs that are beyond the current scope of the project:

* If the user input types more characters than fit on the screen and then backspaces them, backspace will work but not display on the screen
* In some cases freeing memory may not have sufficient resources and thus may cause an irrecoverable lock up
* If the stack over flows there will be a triple fault resulting in an OS reset
    * This requires user mode to be fixed, which is future work
