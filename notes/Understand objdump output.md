# Understand objdump output

See more on:

- [Compiler Explorer](https://godbolt.org/)
- [Understanding how EIP (RIP) register works?](https://stackoverflow.com/questions/27429026/understanding-how-eip-rip-register-works)
- [What are the meanings of callq command parameters?](https://stackoverflow.com/questions/56122039/what-are-the-meanings-of-callq-command-parameters)
- [Assembly - x86 call instruction and memory address?](https://stackoverflow.com/questions/31818870/assembly-x86-call-instruction-and-memory-address)
- [understanding x86-64 call operand](https://stackoverflow.com/questions/58671980/understanding-x86-64-call-operand)
- [What is PLT/GOT?](https://reverseengineering.stackexchange.com/questions/1992/what-is-plt-got)
- [Why are GOT and PLT still present in Linux static stripped binaries?](https://reverseengineering.stackexchange.com/questions/2172/why-are-got-and-plt-still-present-in-linux-static-stripped-binaries)
- [zSeries ELF Application Binary Interface Supplement](https://refspecs.linuxfoundation.org/ELF/zSeries/lzsabi0_zSeries/x1633.html)
- [Why are global variables in x86-64 accessed relative to the instruction pointer?](https://stackoverflow.com/questions/56262889/why-are-global-variables-in-x86-64-accessed-relative-to-the-instruction-pointer)
- [CppCon 2018: Matt Godbolt “The Bits Between the Bits: How We Get to main()”](https://www.youtube.com/watch?v=dOfucXtyEsU&t=20m)
- [Why compiler point to global variable using [rip+0x00]](https://stackoverflow.com/questions/77161481/why-compiler-point-to-global-variable-using-rip0x00)

`objdump` can do quite a bit of task, but in this case, we want to use `objdump` to investigate the assembly code output. Let's say we have three files like this

```c
// add.h
#pragma once
int add(int a, int b);
```

```c
// add.c
#include "add.h"
int add(int a, int b){
    return a + b;
}
```

```c
//main.c
#include "add.h"
#include "stdio.h"

int main(){
    int a = add(5,9);
    printf("a value is %d \n", a);
    return 0;
}
```

If we compile them all like this

```bash
gcc -g -c -o add.o add.c
gcc -g -c -o main.o main.c
```

And link them up

```bash
gcc -Wl,-Map=mapper.map -o final add.o main.o
```

We can investigate the object file and see how the compiler and linker work. First, let's look at the `main.o` and see how the compiler works by running `objdump -M intel -d main.o`

```bash
main.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64
   4:   55                      push   rbp
   5:   48 89 e5                mov    rbp,rsp
   8:   48 83 ec 10             sub    rsp,0x10
   c:   be 09 00 00 00          mov    esi,0x9
  11:   bf 05 00 00 00          mov    edi,0x5
  16:   e8 00 00 00 00          call   1b <main+0x1b>
  1b:   89 45 fc                mov    DWORD PTR [rbp-0x4],eax
  1e:   8b 45 fc                mov    eax,DWORD PTR [rbp-0x4]
  21:   89 c6                   mov    esi,eax
  23:   48 8d 05 00 00 00 00    lea    rax,[rip+0x0]        # 2a <main+0x2a>
  2a:   48 89 c7                mov    rdi,rax
  2d:   b8 00 00 00 00          mov    eax,0x0
  32:   e8 00 00 00 00          call   37 <main+0x37>
  37:   b8 00 00 00 00          mov    eax,0x0
  3c:   c9                      leave
  3d:   c3                      ret
```

Look at the `call` instruction in line `16` and `32`. It's very interesting as the `call` just jump straight to the next instruction instead of going to the place it should be. The expectation should be they jump to the `add` or `printf` instruction. Well, this is expected because we have not linked the file, and as such, the compiler does not know where the function `add` or `printf` is located, and as such, it just calls the next instruction. You can also checkout the hex value of the call: it's used `call` and the op code is `e8`, which is near call and expected an offset from the next instruction. The next byte of `e8` is `00`, which means no offset from next instruction, i.e just go to the next instruction

**One small note**: we see the instruction `call   37 <main+0x37>` and question is, what is `<main+0x37>`? Well, from what I can read, it does not mean anything to the processor - it's just to make it more readable. `main+0x37` means the instruction that is `0x37` from `main` address - which is just `0000 + 0x37 = 0x37` line. You can also inspect the hex value and see that the hex value show nothing about the part `<main+0x37>` - `objdump` put there for us to read

Next, let's look at the `final` file which is the result from linking all other object files by running `objdump -M intel -d final` and this is result I got (I only put some important part below - there are more shown by running the command)

```bash
......

Disassembly of section .plt.sec:

0000000000001050 <printf@plt>:
    1050:       f3 0f 1e fa             endbr64
    1054:       ff 25 76 2f 00 00       jmp    QWORD PTR [rip+0x2f76]        # 3fd0 <printf@GLIBC_2.2.5>
    105a:       66 0f 1f 44 00 00       nop    WORD PTR [rax+rax*1+0x0]

Disassembly of section .text:

0000000000001060 <_start>:
    1060:       f3 0f 1e fa             endbr64
    1064:       31 ed                   xor    ebp,ebp
    1066:       49 89 d1                mov    r9,rdx
    1069:       5e                      pop    rsi
    106a:       48 89 e2                mov    rdx,rsp
    106d:       48 83 e4 f0             and    rsp,0xfffffffffffffff0
    1071:       50                      push   rax
    1072:       54                      push   rsp
    1073:       45 31 c0                xor    r8d,r8d
    1076:       31 c9                   xor    ecx,ecx
    1078:       48 8d 3d e2 00 00 00    lea    rdi,[rip+0xe2]        # 1161 <main>
    107f:       ff 15 53 2f 00 00       call   QWORD PTR [rip+0x2f53]        # 3fd8 <__libc_start_main@GLIBC_2.34>
    1085:       f4                      hlt
    1086:       66 2e 0f 1f 84 00 00    cs nop WORD PTR [rax+rax*1+0x0]
    108d:       00 00 00 

.......

0000000000001149 <add>:
    1149:       f3 0f 1e fa             endbr64
    114d:       55                      push   rbp
    114e:       48 89 e5                mov    rbp,rsp
    1151:       89 7d fc                mov    DWORD PTR [rbp-0x4],edi
    1154:       89 75 f8                mov    DWORD PTR [rbp-0x8],esi
    1157:       8b 55 fc                mov    edx,DWORD PTR [rbp-0x4]
    115a:       8b 45 f8                mov    eax,DWORD PTR [rbp-0x8]
    115d:       01 d0                   add    eax,edx
    115f:       5d                      pop    rbp
    1160:       c3                      ret

0000000000001161 <main>:
    1161:       f3 0f 1e fa             endbr64
    1165:       55                      push   rbp
    1166:       48 89 e5                mov    rbp,rsp
    1169:       48 83 ec 10             sub    rsp,0x10
    116d:       be 09 00 00 00          mov    esi,0x9
    1172:       bf 05 00 00 00          mov    edi,0x5
    1177:       e8 cd ff ff ff          call   1149 <add>
    117c:       89 45 fc                mov    DWORD PTR [rbp-0x4],eax
    117f:       8b 45 fc                mov    eax,DWORD PTR [rbp-0x4]
    1182:       89 c6                   mov    esi,eax
    1184:       48 8d 05 79 0e 00 00    lea    rax,[rip+0xe79]        # 2004 <_IO_stdin_used+0x4>
    118b:       48 89 c7                mov    rdi,rax
    118e:       b8 00 00 00 00          mov    eax,0x0
    1193:       e8 b8 fe ff ff          call   1050 <printf@plt>
    1198:       b8 00 00 00 00          mov    eax,0x0
    119d:       c9                      leave
    119e:       c3                      ret
```

Now, we see that, after linked, in`<main>` we see that the `call` actually jump to the function we expected: `call   1149 <add>` jump to the location of our `add` function, while `call   1050 <printf@plt>` jump to location of `printf` function - which will be linked dynamically when the program is loaded/executed

## Global variable

Now let's tweak the `main.c` a little bit and focus on the global variable information

```c
// main.c
int b = 7;
int c = 3;

int main(){
    int a = 5 + b;
    int d = 6 + c;
    return 0;
}
```

You will get this result

```bash
Disassembly of section .text:

0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64
   4:   55                      push   rbp
   5:   48 89 e5                mov    rbp,rsp
   8:   8b 05 00 00 00 00       mov    eax,DWORD PTR [rip+0x0]        # e <main+0xe>
   e:   83 c0 05                add    eax,0x5
  11:   89 45 f8                mov    DWORD PTR [rbp-0x8],eax
  14:   8b 05 00 00 00 00       mov    eax,DWORD PTR [rip+0x0]        # 1a <main+0x1a>
  1a:   83 c0 06                add    eax,0x6
  1d:   89 45 fc                mov    DWORD PTR [rbp-0x4],eax
  20:   b8 00 00 00 00          mov    eax,0x0
  25:   5d                      pop    rbp
  26:   c3                      ret
```

Here we can see a weird thing here: we access the global variable by using `[rip+0x0]`. However, if we think about it, it does not make any sense as `rip+0x0` refer to the next instruction, and next instruction is not a value. Well, the linker will do the relocation part for you. Just like above, the `00 00 00 00` address of the instruction will be replaced by actual address by the linker. There is a very good talk by Matt Godbolt about this [CppCon 2018: Matt Godbolt “The Bits Between the Bits: How We Get to main()”](https://www.youtube.com/watch?v=dOfucXtyEsU&t=20m)

Actually, you can see the relocation that the linker will do by running with `--reloc` (or `-r` equivalent)

```bash
objdump --reloc -M intel -d main.o
```

And this is what you get

```bash
main.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:
   0:   f3 0f 1e fa             endbr64
   4:   55                      push   rbp
   5:   48 89 e5                mov    rbp,rsp
   8:   8b 05 00 00 00 00       mov    eax,DWORD PTR [rip+0x0]        # e <main+0xe>  a: R_X86_64_PC32        b-0x4
   e:   83 c0 05                add    eax,0x5
  11:   89 45 f8                mov    DWORD PTR [rbp-0x8],eax
  14:   8b 05 00 00 00 00       mov    eax,DWORD PTR [rip+0x0]        # 1a <main+0x1a>        16: R_X86_64_PC32       c-0x4
  1a:   83 c0 06                add    eax,0x6
  1d:   89 45 fc                mov    DWORD PTR [rbp-0x4],eax
  20:   b8 00 00 00 00          mov    eax,0x0
  25:   5d                      pop    rbp
  26:   c3                      ret
```

We see that the linker is called (at line 8) to find where symbol `b` is defined, subtract that by `0x04` and "poked" it in using this `R_X86_64_PC32` at address `a`. In other words, put the instruction right here in this address `a` (which if you notice, is the first byte `00` after `83 05`). For details about how the relocation work out, see [What do R_X86_64_32S and R_X86_64_64 relocation mean?](https://stackoverflow.com/questions/6093547/what-do-r-x86-64-32s-and-r-x86-64-64-relocation-mean/33289761#33289761) - make sure to check out the two links the answer gave. In short: ELF file has a section `.rela.text` that specify the relocation table. This table has info about the location of text that the symbol should replace and where the value for the symbol located. See [How does C++ linking work in practice? [duplicate]](https://stackoverflow.com/questions/12122446/how-does-c-linking-work-in-practice/30507725#30507725)