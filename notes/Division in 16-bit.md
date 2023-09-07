# Division in 16 bits

See work-around implementation [Open Watcom v2 i8d086](https://github.com/open-watcom/open-watcom-v2/blob/master/bld/clib/cgsupp/a/i8d086.asm). Explanation detail see [Building an OS - 5 - How to implement printf](https://www.youtube.com/watch?v=dG8PV6xqm4s&t=1230s)

Look at this code

```c
void main(){
    uint64_t a = 10;
    uint32_t b = 5;
    uint32_t c = a % b;
}
```

You would expect this to work just fine, however, when compile this code to 16-bit processor using Open Watcom `wcc` and `wlink` this error

```bash
Error! E2028: __U8DR is an undefined reference
Warning! W1014: stack segment not found
file /media/nini/Dev/other/os_building/build/stage2/c/stdio.obj(/media/nini/Dev/other/os_building/src/05/bootloader/stage2/stdio.c): undefined symbol __U8DR
```

This code makes absolutely no sense, but through comment and uncomment the code (yes, you need to do that), the problem is with the limitation of x86 code in performing division. If you take a look at the specification of the `DIV` instruction in [Intel x86 manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html#combined), it's detailed

> Divides unsigned the value in the AX, DX:AX, EDX:EAX, or RDX:RAX registers (dividend) by the source operand (divisor) and stores the result in the AX (AH:AL), DX:AX, EDX:EAX, or RDX:RAX registers. The source operand can be a general-purpose register or a memory location. The action of this instruction depends on the operand size
(dividend/divisor). **Division using 64-bit operand is available only in 64-bit mode.**
> The remainder is always less than the divisor in magnitude. Overflow is indicated with the #DE (divide error) exception rather than with the CF flag

As we are in 16-bit processor, we cannot use 64-bit operand and the most we can use is 32-bit operand. However, with the remainder operation `rem = number % radix`, the **compiler will cast all operands to largest type OF THE TWO**. This means, for our example, because `a` is 64-bit wide, compiler will case both `a` and `b` to be 64-bit wide. As such, we will perform the division between two 64-bit number - which is not allowed in 16-bit processor. You can test this out by compile the above code into assembly code by running

```bash
g++ -S -masm=intel -fverbose-asm -g -O0 main.c -o main.s
```

You will see that all are using 64-bit register. If you are using Open Watcom, you can do with the `wdis` to [disassemble the object file](https://stackoverflow.com/questions/50303711/c-source-code-watcom-compiler-and-emu8086)

```bash
/usr/bin/watcom/binl/wcc -4 -d3 -s -wx -ms -zq -fo main.c
/usr/bin/watcom/binl/wdis -l main.o
```

Quick note: Open Watcom `wcc` will call internal function `__U8DR` to perform 64-bit division that "ignore" the restriction on 64 bit divisor. But we need to link them properly