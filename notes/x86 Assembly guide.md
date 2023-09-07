# x86 Assembly guide

From [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html). The first thing to learn is that, unlike modern language, x86 will execute all the code unless specified otherwise. What this means is that, regardless of whether we put code inside a label or not (like `my_func` or `.branch_here`), the instructions will always be written out to the memory. And computer just go through instruction, line by line. Hence, the need to explicitly tell computer which line to go to, and which line to not go to. The label we have (like `my_func` or `.branch_here`) give us convenient tools to tell computer to go to certain line without having to use the explicit address

For some document on NASM directive, see [The Netwide Assembler: NASM](https://www.nasm.us/xdoc/2.13.03/html/nasmdoc6.html). For detail architecture of x86, see [Intel manual, chapter 3](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html#combined)

## Syntax

Although we only have one general x86 (with flavor of course - like x86, x86_64, etc. but all use same language) we have two main syntax: [Intel syntax and AT&T syntax](https://en.wikipedia.org/wiki/X86_assembly_language)

```asm
; AT&T syntax
movl $5, %eax
; Intel syntax
mov eax, 5
```

Beside syntax difference, there is no difference in the underlying architecture, meaning all register and how some instructions work are the same in both syntax. And they will all produce the same machine code

## Register

Modern (i.e 386 and beyond) x86 processors have eight 32-bit general purpose registers. Some registers have subsections. For example, the least significant 2 bytes of `EAX` can be treated as a 16-bit register called `AX`. The least significant byte of `AX` can be used as a single 8-bit register called `AL`, while the most significant byte of AX can be used as a single 8-bit register called `AH`. These names refer to the same physical register, this means when a two-byte quantity is placed into `DX`, the update affects the value of `DH`, `DL`, and `EDX`

![Figure 1. x86 Registers](x86-registers.png)

Next, let talk about notation. Quite often we see in the document stuff like this

>DIV—Unsigned Divide
>...
>Divides unsigned the value in the AX, DX:AX, EDX:EAX, or RDX:RAX registers (dividend) by the source operand
(divisor) and stores the result in the AX (AH:AL), DX:AX, EDX:EAX, or RDX:RAX registers

What those `DX:AX` means is that they are a pair of registers. This is needed to store data that is bigger than size of a single register. For x86, general purpose registers are 32 bits wide. If we want to store a 64-bit data like an `uint64_t`, we need to use 2 registers. When we denote `DX:AX` we means to use `DX` to store the most significant bits in `DX` and least significant bits in `AX`. See [Quick, beginner MASM register question - DX:AX](https://stackoverflow.com/questions/2667899/quick-beginner-masm-register-question-dxax)

## Memory and Addressing Modes

### Registers

See [x86 Registers](https://www.eecg.utoronto.ca/~amza/www.mindsec.com/files/x86regs.html)

About Segment registers like `CS` and `DS`. These register hold the address of the important stuff like `CS` Code segment (i.e where is your instructions is at, also know as Text segment) or `DS` Data segment (i.e where your data segment is at). See [How are the segment registers (fs, gs, cs, ss, ds, es) used in Linux?](https://reverseengineering.stackexchange.com/questions/2006/how-are-the-segment-registers-fs-gs-cs-ss-ds-es-used-in-linux). See memory management section below

### Memory management

See [Memory management](https://wiki.osdev.org/Memory_management)

In short, we have virtual memory address (the one we refer and use in our code) and physical memory. The translation between the virtual and physical address spaces is handled by memory management unit (MMU)

The physical address space is the direct memory address used to access a real location in RAM. The addresses used in this space are the bit patterns used to identify a memory location on the address bus

There are two methods for MMU: paging and segmentation, each use a very different system to manage memory mapping

#### Memory Segmentation

See more in [Memory segmentation](https://en.wikipedia.org/wiki/Memory_segmentation) and [Segmentation](https://wiki.osdev.org/Segmentation)

In a system using segmentation, computer memory addresses consist of a segment id and an offset within the segment.[3] A hardware memory management unit (MMU) is responsible for translating the segment and offset into a physical address, and for performing checks to make sure the translation can be done and that the reference to that segment and offset is permitted.

Each segment has a length and set of permissions (for example, read, write, execute) associated with it. A process is only allowed to make a reference into a segment if the type of reference is allowed by the permissions, and if the offset within the segment is within the range specified by the length of the segment. Otherwise, a hardware exception such as a segmentation fault is raised

This can be used together with memory paging, though segmentation is considered obsolete

#### Memory Paging

See more in [Paging](https://wiki.osdev.org/Paging)
and [Memory Paging](https://en.wikipedia.org/wiki/Memory_paging)

Paging is a system which allows each process to see a full virtual address space, without actually requiring the full amount of physical memory to be available or present. 32-bit x86 processors support 32-bit virtual addresses and 4-GiB virtual address spaces, and current 64-bit processors support 48-bit virtual addressing and 256-TiB virtual address spaces

Paging works by mapping memory through a series of tables, two to be exact. They are the paging directory (PD), and the paging table (PT). Assuming we are working with 32-bit memory address, , both tables contain 1024 4-byte entries, making them 4 KiB each. In the page directory, each entry points to a page table. In the page table, each entry points to a 4 KiB physical page frame. Additionally, each entry has bits controlling access protection and caching features of the structure to which it points. The entire system consisting of a page directory and page tables represents a linear 4-GiB virtual memory map. In case you have different memory address size (like 16 bits or 64 bits) the details will be different. See [How to calculate page frames in a 16 bit address pointer](https://stackoverflow.com/questions/14200025/how-to-calculate-page-frames-in-a-16-bit-address-pointer) or [In a machine using 16 bit addresses and page size is 512 addresses, what's the maximum size for a process that executes?](https://stackoverflow.com/questions/16270220/in-a-machine-using-16-bit-addresses-and-page-size-is-512-addresses-whats-the-m)

Translation of a virtual address into a physical address first involves dividing the virtual address into three parts: the most significant 10 bits (bits 22-31) specify the index of the page directory entry, the next 10 bits (bits 12-21) specify the index of the page table entry, and the least significant 12 bits (bits 0-11) specify the page offset. The then MMU walks through the paging structures, starting with the page directory, and uses the page directory entry to locate the page table. The page table entry is used to locate the base address of the physical page frame, and the page offset is added to the physical base address to produce the physical address. If translation fails for some reason (entry is marked as not present, for example), then the processor issues a page fault

### Declaring Static Data Regions

You can declare static data regions (analogous to global variables) in x86 assembly. Data declarations should be preceded by the `.DATA` directive.Following this directive, the directives `DB`, `DW`, and `DD` can be used to declare one, two, and four byte data locations, respectively. Declared locations can be labeled with names for later reference — this is similar to declaring variables by name, but abides by some lower level rules. For example, locations declared in sequence will be located in memory next to one another. 

```asm
.DATA   
var DB 64    ; Declare a byte, referred to as location var, containing the value 64.
var2  DB ?  ; Declare an uninitialized byte, referred to as location var2.
 DB 10  ; Declare a byte with no label, containing the value 10. Its location is var2 + 1.
X  DW ?  ; Declare a 2-byte uninitialized value, referred to as location X.
Y  DD 30000      ; Declare a 4-byte value, referred to as location Y, initialized to 30000.
```

Arrays in x86 assembly language are simply a number of cells located contiguously in memory. An array can be declared by just listing the values, as in the first example below. Two other common methods used for declaring arrays of data are the DUP directive and the use of string literals. The DUP directive tells the assembler to duplicate an expression a given number of times. For example, `4 DUP(2)` is equivalent to `2, 2, 2, 2`

### Addressing Memory

Modern x86-compatible processors are capable of addressing up to 2^32 bytes of memory: memory addresses are 32-bits wide. In the examples above, where we used labels to refer to memory regions, these labels are actually replaced by the assembler with 32-bit quantities that specify addresses in memory. The address access can be thought to be similar to pointer access in C/C++

Some examples of mov instructions using address computations are:

```asm
mov eax, [ebx]  ; Move the 4 bytes in memory at the address contained in EBX into EAX
mov [var], ebx  ; Move the contents of EBX into the 4 bytes at memory address var. (Note, var is a 32-bit constant).
mov eax, [esi-4]  ; Move 4 bytes at memory address ESI + (-4) into EAX
mov [esi+eax], cl  ; Move the contents of CL into the byte at address ESI+EAX
mov edx, [esi+4*ebx]      ; Move the 4 bytes of data at address ESI+4*EBX into EDX
```

Some examples of invalid address calculations include:

```asm
mov eax, [ebx-ecx]  ; Can only add register values
mov [eax+esi+edi], ebx      ; At most 2 registers in address computation
```

In short, `[ebx]` refer to the content of the address stored in `ebx` while `ebx` alone refer to the content stored in `ebx`

Quick note: the memory address `[ebx + 4]` can also be expressed to be `4[ebx]` - they means the same. For example

```asm
mov eax, [esi-4]
; the above is the same as
move eax, -4[esi]
```

Another example

```asm
mov eax, [esi+8]
; the above is the same as
move eax, 8[esi]
```

## Instruction

For this, the guide is clear enough. See [x86 Instructions](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html). One really big note is that, all the rules below is **by convention**: in Assembly, there are really no rules like which register to store the return value or how stack is used. But following conventions allows the code to be used with other Assembly code or C/C++ code

One reminder is that the stack grow downward - meaning that it grows toward lower memory address. This means that to move the stack pointer upward you decrease its value.

The returned value of a subroutine call, by convention, is stored into `EAX`. The caller can access this later for the returned value - you must do the storing yourself

In the callee, when all are done you must do `ret` in order to go back to the caller. This is needed, because when the caller start the subroutine with `call <subroutine>` it stores address of the instruction right after the `call` to the stack. The `ret` signal the computer to go the instruction store there - you don't need to modify the stack by yourself to do this - though make sure you don't mess up this location. In the figure below, this address is in the `return address`

The `esp` is the stack pointer that point to the top of the stack. The `ebp` is the base pointer and by convention is usually set to `esp` at the start of the function. Functions parameters and local variables are accessed by by adding and subtracting, respectively, a constant offset from `ebp`

![Stack during Subroutine (function) call](stack-convention.png)

For more, see [What is exactly the base pointer and stack pointer? To what do they point?](https://stackoverflow.com/questions/1395591/what-is-exactly-the-base-pointer-and-stack-pointer-to-what-do-they-point)

Next, when we want to have local variable for a subroutine (i.e when registers are not enough), we allocate space in the stack. This is done by moving the base pointer `esp` down by the amount we need. Note that the amount must be a multiple of 4 because each memory sector in x86 is  32 bits (4 bytes). You must manage the `esp` by yourself

## Debugging with Bochs

[Bochs](https://bochs.sourceforge.io/) is a highly portable open source IA-32 (x86) PC emulator written in C++, that runs on most popular platforms. It includes emulation of the Intel x86 CPU, common I/O devices, and a custom BIOS

See [Building an OS - 2 - Reading from the disk](https://www.youtube.com/watch?v=srbnMNk7K7k&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN&index=2) too see how to use Bochs to debug a x86 program that emulate BIOS

## ORG and JMP

### JMP

There ar two forms of `JMP`: short jump and long jump. Short jump will direct to the address that is of relative `offset` to the current sector. Long jump will jump to the correct absolute address of `sector:offset`

```asm
# jump to sector 0xACDC and go to offset 0x5578
JMP 0xACDC:0x5578
# jump to offset 0x5578 of current sector
JMP 0x5578
```

See [Wikipedia JMP (x86 instruction)](https://en.wikipedia.org/wiki/JMP_(x86_instruction)) and [OSDev Segmentation](https://wiki.osdev.org/Segmentation)

### ORG

See [nasm - confusion regarding functionality of ORG directive](https://forum.osdev.org/viewtopic.php?f=1&t=27678) and [What does [ORG 0x7C00] actually do to the file?](https://stackoverflow.com/questions/46811232/what-does-org-0x7c00-actually-do-to-the-file) and [x86 NASM 'org' directive meaning](https://stackoverflow.com/questions/8140016/x86-nasm-org-directive-meaning)

`ORG` defines where the program in question EXPECTS to be loaded into memory. Put it in another way: `ORG` tell assembler where we expect our instruction (i.e the text segment) to be loaded. The assembler will uses this information to calculate label address

Let's go deeper. First, we must understand that the compiled file (the file we get after assembling our assembly code) contain more than instruction for the computer. As such, the first instruction in the file may not be at the beginning of the file. And if this is the case, how can the computer know where to start execution?

For simple program, the assembler does this well enough that we don't need to worry about. In more complex case, this is not the case. For example, let's say we are writing a bootloader for a kernel. The bootloader load the kernel file on a specific memory address. Now the kernel code address will not be correct: after all, without `ORG` the kernel code has no way to control the address of its labels

`ORG` will offset the label with the specified offset. In other word, it will try to put the instruction right at the place it should be