# Overview of OS

A very good guide on Operating System and how to build them [OSDev.org](https://wiki.osdev.org/Expanded_Main_Page)
And a very good intro video is [Building an OS - 1 - Hello world](https://www.youtube.com/watch?v=9t-SPC7Tczc&list=PLFjM7v6KGMpiH2G-kT781ByCNC_0pKpPN)

## How computer find the code for OS

There are two ways for a computer to find the OS:

1. Legacy booting (BIOS)

    This is often used in old, legacy computer. The computer load the **first sector** of every bootable device (like flash drive (USB stick), hard drive, or SSD, etc.) into memory. The computer then search for a signature sequence among the loaded data, and once it found the signature, it will execute the code there

    For detail, see [Boot Sequence](https://wiki.osdev.org/Boot_Sequence)

2. (Unified) Extensible Firmware Interface (UEFI or just EFI)

    The computer look for certain EFI partition on each devices. These are special instruction on how to run the OS. The OS must be compiled as EFI program. Most modern computers use EFI to run the OS

BIOS and UEFI is not mutually exclusive of each other: most device, even modern one, still support booting with BIOS

## Directive vs Instruction

Directive tells the assembler how to compile the program. The directive is not translated to machine code. For example, `function` tell the assembler that the next syntax is a function, or `.word` tell computer that the variable is of word size. This is specific to assembler

Instruction translated into instruction (i.e machine code) that tell the computer what to do. For example, `add` will add two values and write the result into another register