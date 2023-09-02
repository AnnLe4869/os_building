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

### Format for FAT file system

The device/image file that have FAT file system must have the correct bytes sequence at the start of the device/image. See [OSDEv implementation detail](https://wiki.osdev.org/FAT#Implementation_Details). If you are using Assembly, here is the setup

```asm
;
; FAT12 header
; 
jmp short start
nop

bdb_oem:                    db 'MSWIN4.1'           ; 8 bytes
bdb_bytes_per_sector:       dw 512                  ; because all numbers are in the little-endian format
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0F0h                 ; F0 = 3.5" floppy disk
bdb_sectors_per_fat:        dw 9                    ; 9 sectors/fat
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record for FAT 12
ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
                            db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
ebr_volume_label:           db 'HELLO OS   '        ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12   '           ; 8 bytes
```

Notice how we write bytes sequence into the drive. These are all directive and are default to be put at the beginning of the compiled file (i.e when we compile the file into instruction). It would be very helpful if you format a test image to check what is the setup of a typical FAT look like. If this all go well, we can check whether this actually works or not by running

```bash
# the -i is need to specify the image/device to be used as input
mdir -i your_file.img

Volume in drive : is NBOS       
Volume Serial Number is 7226-5612
Directory for ::/

kernel   bin       512 2022-08-02   6:53 
        1 file                  512 bytes
                          1 457 152 bytes free


```

## Disk layout

For hard drive, we have multiple platters - the circular disk in the drive. We can read/write to both sides of the platter, and to do this, we need two heads for each platter: one head for the top side, one head for bottom side of the platter. A platter can be divided into sector and track/cylinder in which we store data upon (see image below)

![Hard drive layout](Cylinder_Head_Sector.png)

There are two ways to address a block: Cylinder-head-sector (CHS) and Logical block addressing (LBA). LBA is the modern way of addressing the block on a disk, and  the formula to convert from LAB to CHS is as such (from [OSDev LBA](https://wiki.osdev.org/LBA))

```c
// this assume that both side of a platter can be read/write
// which means each platter has two heads
void ToCHS(int lba, int *head, int *track, int *sector)
{
        (*head) = (lba % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK;
        (*track) = (lba / (SECTORS_PER_TRACK * 2));
        (*sector) = (lba % SECTORS_PER_TRACK + 1);
}
```

Or you can check out the formula in [Wikipedia LBA](https://en.wikipedia.org/wiki/Logical_block_addressing)