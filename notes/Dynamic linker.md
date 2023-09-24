# Dynamic linker

See more on:

- [Where do executables look for shared objects at runtime?](https://unix.stackexchange.com/questions/22926/where-do-executables-look-for-shared-objects-at-runtime)
- [Wikipedia Linker (computing)](https://en.wikipedia.org/wiki/Linker_(computing))
- [Wikipedia Shared Library](https://en.wikipedia.org/wiki/Shared_library)
- [Wikipedia Dynamic Linker](https://en.wikipedia.org/wiki/Dynamic_linker)
- [OSDev Dynamic Linker](https://wiki.osdev.org/Dynamic_Linker)
- [Redhat Chapter 16. Using Libraries with GCC](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html/developer_guide/gcc-using-libraries)
- [How dynamic linking for modular libraries works on Linux](https://opensource.com/article/22/5/dynamic-linking-modular-libraries-linux)
- [Is the dynamic linker automatically invoked by the operating system or by code embedded in the ELF file?](https://unix.stackexchange.com/questions/539352/is-the-dynamic-linker-automatically-invoked-by-the-operating-system-or-by-code-e)
- [What is the linker and what is the loader in dynamic linking?](https://unix.stackexchange.com/questions/611733/what-is-the-linker-and-what-is-the-loader-in-dynamic-linking)
- [How programs get run: ELF binaries](https://lwn.net/Articles/631631/)
- [CppCon 2018: Matt Godbolt “The Bits Between the Bits: How We Get to main()”](https://www.youtube.com/watch?v=dOfucXtyEsU&t=20m)

First of all, dynamic linker can only work when your program is running in a non-freestanding environment. This means the program is running inside an operating system that have some sort of dynamic linker. A dynamic linker is the part of an operating system that loads and links the shared libraries needed by an executable when it is executed (at "run time"), by copying the content of libraries from persistent storage to RAM, filling jump tables and relocating pointers

Take the Linux ELF file as an example. In Linux, the dynamic linker is just an executable program that can be called. When an executable file is loaded, the operating system kernel reads the path of the dynamic linker from it and then attempts to load and execute this other executable binary; if that attempt fails because, for example, there is no file with that path, the attempt to execute the original executable fails. The dynamic linker then loads the initial executable image and all the dynamically-linked libraries on which it depends and starts the executable. As a result, the pathname of the dynamic linker is part of the operating system's application binary interface. 