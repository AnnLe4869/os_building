# Building toolchains

When we want to build our OS, we don't have any libraries, nor any compiler. Thus to use tools like `gcc` or `ld`, we need to compile from sources and put those binary code into our OS. There are several reasons we don't want to just use our host to compile everything and put the binary into our OS we are building:

- `gcc` and `ld` will link the file using our host system. This means our code will, inherently rely on the host machine instead of the OS - which by itself has none of the library. One example is `libc` which is linked with any C file by default
- Configure the target we want to compile/link our code to. By default, if we use our host machine, the code will be compile/link to be executable by the host machine. If we want the code to be executable on another platform (for example, host is `x86_64` while target is 32-bit machine), we need to specify the option on the tools itself

Hence the needs to compile and build the tools ourselves. Two tools we want to use are `binutils` and `gcc`. Our goal is to have a cross compiler that can compile our code into the desired target system. On how to build those two system, check out the video [Setting up an OS dev environment, building GCC toolchain](https://www.youtube.com/watch?v=TgIdFVOV_0U&t=8m). Ignore the part about Windows WSL as we only care about how to install and build `binutils` and `gcc`. Another good video is [Building an OS - 7 - Finishing the bootloader, migrating to GCC](https://www.youtube.com/watch?v=qr8d7bZLPHg)

See [Why do I need a Cross Compiler?](https://wiki.osdev.org/Why_do_I_need_a_Cross_Compiler%3F) and how to build one [OSDev GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross_Compiler). For list of freestanding header, check out [OSDev C library](https://wiki.osdev.org/C_Library)

## Freestanding environment

When you build a program to run on freestanding environment (i.e `gcc -c -ffreestanding -nostdlib`) the program run very different from normal program that run on a normal OS. You need the OS to explicitly tell the program to run, where to run, etc. - which we don't have as we don't have standard library. As such, very easily encounter segfault even when you have nothing in your program. Check this answer [segmentation fault without standard libraries](https://stackoverflow.com/questions/72160318/segmentation-fault-without-standard-libraries)