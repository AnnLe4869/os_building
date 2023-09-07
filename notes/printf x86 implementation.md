# printf x86 assembly implementation

First, let take a look at a normal C `printf` usage

```c
printf ("Test %d %u %x %i", 1, 2, 3, 4);
```

If `printf` is implemented in x86, we should string itself should be the last element being pushed to the stack (to be precise, it's the pointer to the string being pushed to the stack) while the last argument `4` is the first thing being pushed to the stack

This is very convenient for us because we have access to the thing we interest most first, and thing we interest least last. From the callee perspective, it has access to the string pointer first. Then all the callee need to do is go through the string one character at a time, and every time it encounter a [format specifier](https://cplusplus.com/reference/cstdio/printf/), it will look for the next parameter

Let's use our example, assuming we are in 32-bit processor

- The pointer to the string is at `ebp + 8`. We go through each character, one at a time
- When encounter `%d`, look for the next parameter - in this case, `ebp + 12`
- Move on. Encounter `%u`, look for the next parameter - in this case it is `ebp + 16`
- Move on. Encounter `%x`, look for the next parameter - in this case it is `ebp + 20`
- Move on. Encounter `%i`, look for the next parameter - in this case it is `ebp + 24`

As you can see, we just keep moving up the stack (i.e move toward higher address) every time we encounter a format specifier