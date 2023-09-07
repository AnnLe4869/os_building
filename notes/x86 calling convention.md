# x86 calling convention

See [x86 Disassembly/Calling Conventions](https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions) and [x86 Assembly Guide](https://www.cs.virginia.edu/~evans/cs216/guides/x86.html) and [Wikipedia x86 calling convention](https://en.wikipedia.org/wiki/X86_calling_conventions)

Calling conventions are a standardized method for functions to be implemented and called by the machine. This is needed so that your Assembly code can be used together with C/C++ code

The C language, by default, uses the CDECL calling convention.In the CDECL calling convention the following holds:

- Arguments are passed on the stack in Right-to-Left order, and return values are passed in `eax` register
- The calling function cleans the stack
- The caller-saved registers are `EAX`, `ECX`, `EDX`. Since the called subroutine is allowed to modify these registers, if the caller relies on their values after the subroutine returns, the caller must push the values in these registers onto the stack
- When translated to assembly code, CDECL functions are almost always prepended with an underscore

Note that depending on the architecture, the size of register is different. For example, 16 bits architecture register is 16 bits in side(2 bytes), 32 bits is 4 bytes. You will need this to calculate the calling function

## Notes on memory and register

Another note is that, while calling function, we have to make sure to preserve the base pointer `ebp`. Base pointer `ebp` tell processor where our local variable is - this is different from the return address. The return address, as see in the picture below, tell the processor where to go back to after finish current function (i.e when we call `ret`) whereas `ebp` tells us where is the base of the stack of our function is. When we call a new function in the middle of another function, when the other function finish, if we don't save a copy of `ebp` how can we know where is the base of the original function stack is?

![Calling convention for 32 bits machine](stack-convention.png)

The base pointer is used by convention as a point of reference for finding parameters and local variables on the stack. When a subroutine is executing, the base pointer holds a copy of the stack pointer value from when the subroutine started executing. Parameters and local variables will always be located at known, constant offsets away from the base pointer value

The stack is growing down, meaning the stack grow toward lower addresses. This means to access local variable we do like `[ebp - 4]` while accessing parameter we do like `[ebp + 7]`

## Example

Consider the following C instructions (32 bits)

```c
_cdecl int MyFunction1(int a, int b)
{
  return a + b;
}
```

and the following function call:

```c
x = MyFunction1(2, 3);
```

These would produce the following assembly listings, respectively

```asm
_MyFunction1:
    push ebp            ; save the old ebp pointer
    mov ebp, esp        ; ebp point to the stack pointer, before stack change size - like more room for local variable
    mov eax, [ebp + 8]
    mov edx, [ebp + 12]
    add eax, edx
    pop ebp             ; recover caller base pointer value
    ret
```

```asm
push 3                  ; caller pass variable by pushing to stack
push 2                  ; push to stack from right to left  
call _MyFunction1
add esp, 8              ; caller isn responsible for restoring stack
```

Notice the `_cdecl` here. This directive is needed so that the compiler and linker can learn that `MyFunction1` is a function that maybe used elsewhere that is not C. For example, if we want to use this in another Assembly code we can do like this

```asm
call _MyFunction1
```

and this should produce the same result regardless of whether we wrote the function in C or in Assembly - as long as we follow the convention (`_cdecl` for C, etc.)
