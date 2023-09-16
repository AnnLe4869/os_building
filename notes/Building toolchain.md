# Building toolchains

When we want to build our OS, we don't have any libraries, nor any compiler. Thus to use tools like `gcc` or `ld`, we need to compile from sources and put those binary code into our OS. There are several reasons we don't want to just use our host to compile everything and put the binary into our OS we are building:

- `gcc` and `ld` will link the file using our host system. This means our code will, inherently rely on the host machine instead of the OS - which by itself has none of the library. One example is `libc` which is linked with any C file by default
- Configure the target we want to compile/link our code to. By default, if we use our host machine, the code will be compile/link to be executable by the host machine. If we want the code to be executable on another platform (for example, host is `x86_64` while target is 32-bit machine), we need to specify the option on the tools itself

Hence the needs to compile and build the tools ourselves. Two tools we want to use are `binutils` and `gcc`. Our goal is to have a cross compiler that can compile our code into the desired target system. On how to build those two system, check out the video [Setting up an OS dev environment, building GCC toolchain](https://www.youtube.com/watch?v=TgIdFVOV_0U&t=8m). Ignore the part about Windows WSL as we only care about how to install and build `binutils` and `gcc`