#pragma once

// 0x00000000 - 0x000003FF - interrupt vector table
// 0x00000400 - 0x000004FF - BIOS data area

// usable memory area - conventional memory and OS Boot sector go here
#define MEMORY_MIN          0x00000500
#define MEMORY_MAX          0x00080000

// 0x00000500 - 0x00010500 - FAT driver
// in other words, FAT data region
// may need more for bigger drive
#define MEMORY_FAT_ADDR     ((void far*)0x00500000)         // segment:offset (SSSSOOOO)
#define MEMORY_FAT_SIZE     0x00010000          // 64 KB

/**
 * why choose 0x00000500 - 0x00010500
 * because this is after the code segment for stage 2, \
 * and we give ample space for some local variables (i.e stack)
 * 
 */

// 0x00020000 - 0x00030000 - stage2

// 0x00030000 - 0x00080000 - free

// 0x00080000 - 0x0009FFFF - Extended BIOS data area
// 0x000A0000 - 0x000C7FFF - Video
// 0x000C8000 - 0x000FFFFF - BIOS