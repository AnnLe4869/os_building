# x86 Assembly guide

From [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html)

## Register

Modern (i.e 386 and beyond) x86 processors have eight 32-bit general purpose registers. Some registers have subsections. For example, the least significant 2 bytes of `EAX` can be treated as a 16-bit register called `AX`. The least significant byte of `AX` can be used as a single 8-bit register called `AL`, while the most significant byte of AX can be used as a single 8-bit register called `AH`. These names refer to the same physical register, this means when a two-byte quantity is placed into `DX`, the update affects the value of `DH`, `DL`, and `EDX`

![Figure 1. x86 Registers](x86-registers.png)

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

## Instruction

For this, the guide is clear enough. See [x86 Instructions](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html). One really big note is that, all the rules below is **by convention**: in Assembly, there are really no rules like which register to store the return value or how stack is used. But following conventions allows the code to be used with other Assembly code or C/C++ code

One reminder is that the stack grow downward - meaning that it grows toward lower memory address. This means that to move the stack pointer upward you decrease its value.

The returned value of a subroutine call, by convention, is stored into `EAX`. The caller can access this later for the returned value - you must do the storing yourself

In the callee, when all are done you must do `ret` in order to go back to the caller. This is needed, because when the caller start the subroutine with `call <subroutine>` it stores address of the instruction right after the `call` to the stack. The `ret` signal the computer to go the instruction store there - you don't need to modify the stack by yourself to do this - though make sure you don't mess up this location. In the figure below, this address is in the `return address`

The `esp` is the stack pointer that point to the top of the stack. The `ebp` is the base pointer and by convention is usually set to `esp` at the start of the function. Functions parameters and local variables are accessed by by adding and subtracting, respectively, a constant offset from `ebp`

![Stack during Subroutine (function) call](stack-convention.png)

For more, see [What is exactly the base pointer and stack pointer? To what do they point?](https://stackoverflow.com/questions/1395591/what-is-exactly-the-base-pointer-and-stack-pointer-to-what-do-they-point)

Next, when we want to have local variable for a subroutine (i.e when registers are not enough), we allocate space in the stack. This is done by moving the base pointer `esp` down by the amount we need. Note that the amount must be a multiple of 4 because each memory sector in x86 is  32 bits (4 bytes). You must manage the `esp` by yourself