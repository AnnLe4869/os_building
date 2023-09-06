# Compiling C to 16 bits RealMode

For video guide, check out [Building an OS - 4 - Writing bootloader in C](https://www.youtube.com/watch?v=F3XiH78erNM&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN&index=4)

Normally, we would use `gcc` or `clang` to compile/link C code. However, these compilers don't support 16 bits real mode. We need to use other compilers. For this one, I will use [Open Watcom](https://github.com/open-watcom/open-watcom-v2) as this is quite reliable and free

Download the latest version to your machine. Assuming you are running on a Linux machine (for other, Google is your friend). You can first check whether the file is correct one or not - it should be an executable file (check by running `file <installed_file>`). Then we need to allow it to run using `chmod +x <installed_file>`

Now we can run it just like a normal executable file, though we for convenience run it with admin mode `sudo ./<installed_file>`. This should pop up a screen to install and choose our option. Click on next until you see the option to install either in `Selective installation` or `Full installation`. If you know what you do, you can go selective, else just do the full installation. Continue till the end. The program should be install in `/usr/bin/watcom`

The compiler/linker we will use is `wcc` and `wlink` - this can compile/link 32-bit code. You should check the documentation for this in [Open Watcom Documentation](https://github.com/open-watcom/open-watcom-v2/wiki/Open-Watcom-Documentation). Or this guide for an overview [Open Watcom C/C++](https://flaterco.com/kb/ow.html)

## Compiling details

Compiler's job is to translate C code into object file (think object file is kinda like Assembly code). These file can be read and executed by the processor. Still, because C supports separate compilation of multiple files, the object files generated at this point aren’t completely ready to be executed. In particular, they contain references to identifiers that are defined elsewhere. That is, they use variables and call functions defined in other files. Resolving the references between object files is the job of the linker. In many cases, perhaps most, the linker is a separate program. It takes object files and libraries and resolves the linkage between them. It determines the address that each symbol will be located at and then ensures that references to them from elsewhere in the code use that address. (see [How does a compiler work in C language? What is preprocessing and post processing in the context of a compiler for C language?](https://www.quora.com/How-does-a-compiler-work-in-C-language-What-is-preprocessing-and-post-processing-in-the-context-of-a-compiler-for-C-language))

A compiler can:

- Specify the target instruction. For example, is it for 16 bits, or 32 bits, for 486 instructions or x86_64 instruction, etc.
- Specify the memory model. For example, should the stack and data segment same place, how big is the stack, etc. This will affect how much memory register needed to store a memory location. Think of how memory address is calculated: if data is small, we only need offset to go from one place to another; if data is far from each other, we need the segment and offset value to go from one to the other. See the explanation in [Building an OS - 4 - Writing bootloader in C](https://www.youtube.com/watch?v=F3XiH78erNM&t=13m)
- Specify the error and warning. Should the program throw error and stop, or just stop. Again, error like stack overflow is handled by OS - if we write bootloader then there is no OS and no one stop us

Some important tag when compile C code

- `-s`: remove stack overflow check - a runtime feature, because we are in [free standing environment](https://stackoverflow.com/questions/30825151/is-there-a-meaningful-distinction-between-freestanding-and-hosted-implementation) and don't have the feature implemented yet
- `-wx`: set warning level to maximum
- `-zl`: suppress generation of library file names and references in object file. In other words, do not add reference to standard library, as we don't have them in free standing environment
- `-zq`: operate quietly - means that only display the warning and error
- `-ms`: set the memory model to be small. This is important as smaller model can help the program run faster. See [Building an OS - 4 - Writing bootloader in C](https://www.youtube.com/watch?v=F3XiH78erNM&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN&index=5)

## Linker details

Linker does a lot more job than you probably though. One of those job is to link file together. Another one is to setup the structure of the executable file. Any executable file will need some structure so that the OS can understand and execute it. We can explicitly specify the format to fir our purpose. For example, when we write code for bootloader, we don't follow normal executable file format as there is no OS to execute these - we want just bare bone binary file. See [How to correctly use a simple linker script? Executable gets SIGKILL when run](https://stackoverflow.com/questions/7182409/how-to-correctly-use-a-simple-linker-script-executable-gets-sigkill-when-run) and [Executable and Linkable Format](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format)

See more on [Building an OS - 4 - Writing bootloader in C](https://www.youtube.com/watch?v=F3XiH78erNM&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN&index=5) or [What do linkers do?](https://stackoverflow.com/questions/3322911/what-do-linkers-do)

Some important points for linker:

- We can specify the format of the executable files. For smaller file size, or for fast bootloader, we want to jump straight to the instruction part without having to search for symbol, parse header, etc. This can be done by including a [linker script file](https://users.informatik.haw-hamburg.de/~krabat/FH-Labor/gnupro/5_GNUPro_Utilities/c_Using_LD/ldLinker_scripts.html)
- Adding mapping for debugging purpose