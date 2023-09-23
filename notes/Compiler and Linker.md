# Compiler and Linker

GCC can be used as both compiler and linker. Under the hood, when use `gcc` as linker, it actually calls `ld` (see [What's the Difference between linking by GCC and LD?](https://stackoverflow.com/questions/26152896/whats-the-difference-between-linking-by-gcc-and-ld)). If you run `gcc` without the `-c` flag, it will compile and link the program all at once. In this article, we are interested in the file format of the resulted file

On the other hand, assembler like NASM or GAS only translate assembly code to machine code. This is mechanical, and can be done in only one way (see [Is there any real difference between a compiler and an assembler?](https://cs.stackexchange.com/questions/13904/is-there-any-real-difference-between-a-compiler-and-an-assembler)). Both compiler and assembler will produce executable file (we call resulted file "object file" to distinguish from final resulted file) - this means the processor can read the compiled/assembled file and execute it. However, more often than not we will need linker to really run those programs, as one program may use a function defined in another file

## Compiler

When compile a program using GCC like `gcc -c -o hello.o hello.c`, we compile the file `hello.c` into executable file. The format of the executable file is dependent on the machine you run the `gcc` on. For example, if you run this on Linux machine, `hello.o` is probably is an ELF file, while in Windows it probably is COFF. If you compile using cross-compiler GCC (meaning, you [compile the GCC yourself](https://wiki.osdev.org/GCC_Cross_Compiler)), the output format is the format you specify when you build the compiler. See [How can I make GCC generate ELF object files?](https://stackoverflow.com/questions/52244641/how-can-i-make-gcc-generate-elf-object-files) or [Is there a way to get gcc to output raw binary?](https://stackoverflow.com/questions/1647359/is-there-a-way-to-get-gcc-to-output-raw-binary)

Trying to compile a file into a format different from the one supported by your operating system is often a pain and not worth it. See [How to compile c program into elf format?](https://stackoverflow.com/questions/46552197/how-to-compile-c-program-into-elf-format) and [Compiling C to ELF32 on Windows](https://stackoverflow.com/questions/65752272/compiling-c-to-elf32-on-windows). Why not worth it? Well, if you compile program to run on your machine, then there is no reason to convert to different format. And if for different machine architecture, you can do that better with finer control during the linker step

With assembler however, you can specify the format you want the assembler to output to. For NASM, the default format is binary `bin`. You can specify the output format with the `-f` option (see [NASM The -f Option: Specifying the Output File Format](https://www.nasm.us/xdoc/2.16.01/html/nasmdoc2.html#section-2.1.2))

## Linker

This is the fun part. Linker allows us to combine multiple object files into one final executable file. The linker allows us to share code between different object file: an object file can use a function define in different object file. For example, we can define a function in x86 and use this function in a C code file. The linker also tell the processor how to load and run it. Again, if you want to run the program in OS environment, it needs to be told where to load the program and how to start it. The linker will put all these information onto the header of the final linked file. See [Why do I have to use ld to link my binary](https://stackoverflow.com/questions/58646504/why-do-i-have-to-use-ld-to-link-my-binary)

Think about it: when we compile one file into an object file, although this object file may have detailed about where to load the program section (think of text, data, etc. section), when we combine multiple object files, what should be the address where each section load? What should the address where we can jump to (if you remember, when we refer to a symbol, we basically jump to the address that store the symbol). The linker resolve all these for us See [Wikipedia Linker(computing)](https://en.wikipedia.org/wiki/Linker_(computing)). Note here. If you are programming for bare metal or program an OS bootloader, you may not need a linker as we can control where the program is loaded explicitly in certain memory address, and because we know the processor will always start execute from that memory address, we can manually setup the section address as we want

One important thing to remember is that, the linker is also the program that assigns the memory address for each section of the program. This means, when the program is loaded to memory, the location of where each segment is located in memory was specified by the linker (roughly speaking - or at least, the memory allocated to the program by the kernel). We can technically access the memory location, let's say, `.bss` section and do some stuff with if from the program itself if we use in conjunction with linker script symbol

### Static vs Dynamic Linker

See [What do 'statically linked' and 'dynamically linked' mean?](https://stackoverflow.com/questions/311882/what-do-statically-linked-and-dynamically-linked-mean) and [Static vs. Dynamic Linking](https://www.baeldung.com/cs/static-dynamic-linking-differences)

Linking is the step where we try to resolve all symbols used in our program. Let's say we have program `A` that use library `libB` and we want to get an executable file `exeC`. With static linking, the linker include the library used in our code into the final executable file. Using our example, this means `exeC` will contains the code of `A` and `libB`. 

With dynamic linking however, a pointer to the file being linked in (the file name of the file, for example) is included in the executable and the contents of said file are not included at link time. It's only when you later run the executable that these dynamically linked files are brought in and they're only brought into the in-memory copy of the executable, not the one on disk. Using our example, this means `exeC` will only contain a pointer to `libB`, and when we run `exeC` the `libB` will be brought into memory for `exeC` to use

### Linker script

With linker, we can specify the structure of the final executable file: what is the format of it - is it binary or ELF; the address of each section and what is include in each section - where should the `.text` section be and should it be before or after `.data`, etc. We do this by using [linker script](https://wiki.osdev.org/Linker_Scripts). Check out [Basic Linker Script Concepts](https://sourceware.org/binutils/docs/ld/Basic-Script-Concepts.html) and [Simple Linker Script Example](https://sourceware.org/binutils/docs/ld/Simple-Example.html) and [Linker Script File](http://www.bravegnu.org/gnu-eprog/lds.html)

Additionally, with linker script, we can obtain the address of each section to be used in other program. It is possible to define arbitrary symbols within a linker script. These symbols are added into the program's symbol table. Each symbol in the table has a name and an associated address. Symbols within a linker script that have been assigned a value will be given external linkage, and are accessible within the program's code as pointers

```ld
floating_point = 0;
SECTIONS
{
  .text :
    {
      *(.text)
      _etext = .;
    }
  _bdata = (. + 3) & ~ 3;
  .data : { *(.data) }
}
```

In the above example, the symbol floating_point has been defined as zero. The symbol `_etext` has been defined as the address following the last .text input section. The symbol `_bdata` has been defined as the address following the `.text` output section aligned upward to a 4 byte boundary

An example of usage

```c
/** Externally linked symbol */
extern uintptr_t _etext;
// ...
/** Pointer to the binary data at the address stored in the symbol expression. */
uint32_t* item = &_etext;
```

**Note**: linker will want input files to be of the same format. Because NASM will default into binary, which means the assembled files will often be of different format that files that are compiled using compiler, we often change the NASM option (the `-f` option) so that NASM will assemble the file into a desired format (again, it's easier to change assembler output format than compiler output format)