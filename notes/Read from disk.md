# Read from disk

Currently, all of our instructions for the OS kernel are stuffed into only 512 bytes - the size of a sector. This is not ideal, as 512 bytes is way too small. We cannot go bigger than this without new methods, as by default, computer only recognized the sector that end with `0x55 0xaa` as special boot sector

Our solution would be split the code into two parts: the first part that can be stored in small 512 bytes sector call bootloader; the second part that is bigger and stored in other location is our actual OS kernel code. The bootloader is small and have instructions on how to load the OS kernel code

## Bootloader

The boot loader ultimately has to:

- Bring the kernel (and all the kernel needs to bootstrap) into memory
- Provide the kernel with the information it needs to work correctly
- Switch to an environment that the kernel will like
- Transfer control to the kernel

One option of setting up bootloader would be to put the bootloader in first sector then operating system in second sector. The problem with this is that we wouldn't be able to store any other files aside from OS kernel code in the same disk