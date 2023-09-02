# Linux commands

## gzip and gunzip

`gzip` is used to compress file while `gunzip` is used to decompress file (usually files that are compressed by `gzip`). By default, they both compress/decompress the original file, meaning the original file will be replaced by the compress/decompressed file

## dd

See [When and How to Use the dd Command](https://www.baeldung.com/linux/dd-command). Additional documents are [arch linux dd](https://wiki.archlinux.org/title/Dd)

In the venerable Unix command dd, the disk/data duplicator (or, sometimes, disk destroyer) allows us to **copy** raw data from one source to another. It’s not used to copy individual files like cp. Instead, it lets us read from and write to block devices — for example, physical hard drives. We can use this in tasks like

- copying an image file to a USB boot drive
- reading data off of inaccessible or crashed file systems.

`dd` will copy from input to output file, and put the copy part at the start of the output file.

The `/dev/zero` is a special file in Unix-like operating systems that provides as many null characters (ASCII NUL, 0x00) as are read from it. One of the typical uses is to provide a character stream for initializing data storage

```bash
# Creating a 1 MiB file, called foobar, filled with null characters
dd if=/dev/zero of=foobar count=1024 bs=1024
# Destroying existing data on a file system partition or drive
dd if=/dev/zero of=/dev/<destination drive or partition>
# copy one file to another file
dd if=bootloader.bin of=main_floppy.img conv=notrunc
```

The `if` is the input file and `of` is the output file. Note that the `of` file will be completely replaced with the content of the `if` file

```bash
dd if=/dev/sda of=/dev/sdb
```

This would slowly transfer everything from one disk to another, obliterating everything on the out file disk sdb. Everything there would be gone, replaced by what was on sda

Note that if `sdb` was a larger disk than `sba`, our filesystem would not resize. It would remain an exact raw binary duplicate, so it would look like the smaller source disk

Notable options:

- `bs`: specify [block size](https://www.ibm.com/docs/en/storage-scale/5.1.8?topic=considerations-block-size), i.e In a file system, a block is the largest contiguous amount of disk space that can be allocated to a file and also the largest amount of data that can be transferred in a single I/O operation. The block size determines the maximum size of a read request or write request that a file system sends to the I/O device driver. The default is 512 bytes
- `count`: specify the number of block to copy from input file to output file. For example, `dd if=/dev/zero of=first.img bs=1G count=10` will copy one gigabyte of zeros ten times to `first.img`
- `conv=notrunc` do not truncate the file. This is useful if we want to copy from a smaller disk to bigger disk

One very important aspect of `dd` is that it overwrite everything, even the **file system** on the output file. What this means is that, when you do like `dd if=/dev/zero of=foo.img` the `foo.img` will not have any file system on it. You will have to set the file system there on your own

## mkfs

The `mkfs` command makes a new file system on a specified device. The `mkfs` command initializes the volume label, file system label, and startup block. More often that not, we use a file-type specific like `mkfs.fat` or `mkfs.ntfs` (see [What's the difference between mkfs and mkdosfs?](https://askubuntu.com/questions/87024/whats-the-difference-between-mkfs-and-mkdosfs)). For all file system creation tools, see [Arch Linux file system](https://wiki.archlinux.org/title/File_systems). 

This will focus on `mkfs.fat`. The details can be read in [Arch Linux FAT](https://wiki.archlinux.org/title/FAT) and [mkfs.fat](https://man.archlinux.org/man/mkfs.fat.8)

Essentially, `mkfs.fat` is used to create a FAT filesystem on a device or in an image file. The syntax is

```bash
mkfs.fat [OPTIONS] DEVICE [BLOCK-COUNT]
```

`[BLOCK-COUNT]` is the number of blocks on the device and size of one block is always 1024 bytes, independently of the sector size or the cluster size. Therefore `BLOCK-COUNT` specifies size of filesystem in KiB unit and not in the number of sectors (like for all other mkfs.fat options). If omitted, mkfs.fat automatically chooses a filesystem size to fill the available space.

For example

```bash
# Format a partition to FAT32
mkfs.fat -F 32 /dev/partition
# Format an image file to fat 12 and set the volume name to "HELLO"
mkfs.fat -F 12 -n "HELLO" main_floppy.img
```

Some important options:

- `-F` is used to specify FAT size, like 12, 16 or 32
- `-C` is used to Create the file given as DEVICE on the command line, and write the to-be-created filesystem to it. This can be used to create the new filesystem in a file instead of on a real device, and to avoid using dd in advance to create a file of appropriate size. With this option, the BLOCK-COUNT must be given, because otherwise the intended size of the filesystem wouldn't be known

The file/device you just created is not a valid file system and can be treated as a normal drive. This means you can mount it and use it just like a drive. Oow to do this, see [Arch Linux FAT](https://wiki.archlinux.org/title/FAT)

## mcopy

See more on [Wikipedia Mtools](https://en.wikipedia.org/wiki/Mtools) and [GNU Mcopy](https://www.gnu.org/software/mtools/manual/html_node/mcopy.html) and [Mtools 4.0.43](https://www.gnu.org/software/mtools/manual/mtools.html)

Mtools is an open source collection of tools to allow a Unix operating system to manipulate files on an MS-DOS file system, and `mcopy` is a tool used to copy one file from one file system to another file system. This is very useful, consider this scenario: you are developing your own file system in the image `main_floppy.img` (let's say this is FAT 32 while your OS is using ext4) and now you want to move the file `hello_world.py` into this image file. You may:

- Copy the file using `cp` or `mv`. This will not work as our OS has different file system than `main_floppy.img`.
- Using `dd` to copy the file. This will destroy the `main_floppy.img` as the `dd` when copy will overwrite the drive, including the encoding for the file system
- Mount the image then do the copy. This will work, but is cumbersome as you need to first mount the image - this required admin privilege, then you can move using `cp`

`mcopy` does this copy job for us without the need to mount the image/device file. Additionally, this is two ways: we can copy from the image to our OS file system, and we can copy from our OS file system to the image. The syntax is

```bash
mcopy [-bspanvmQT] [-D clash_option] sourcefile targetfile
mcopy [-bspanvmQT] [-D clash_option] sourcefile [ sourcefiles… ] targetdirectory
mcopy [-tnvm] MSDOSsourcefile
```

`mcopy` copies the specified file to the named file, or copies multiple files to the named directory. The source and target can be either MS-DOS or Unix files

```bash
# copy the local file hello.py to main_floppy.img and name the file namely.py
mcopy -i main_floppy.img hello.py "::namely.py"
```

Try this experiment by first creating a file `hello.txt` with a very short string `hello world` inside and run the script below

```bash
dd if=/dev/zero of=main_floppy.img bs=512 count=2880
mkfs.fat -F 12 -n "NBOS" main_floppy.img
mcopy -i main_floppy.img hello.txt ::jojo.txt
mcopy -i main_floppy.img ::jojo.txt kimi.py
```

At the end, you will see that we have 2 new files

- `main_floppy.img` is the image file with FAT 12 file system
- `kimi.py` have the exact same content as `hello.txt`

What happened is that we copy the file `hello.txt` to `main_floppy.img` and name that file `jojo.txt`, then we copy that file `jojo.txt` from `main_floppy.img` out to our local machine and name the copied one on local `kimi.py`. The tricky one in these commands is the file system format: choose a wrong size, like `mkfs.fat -F 32 -n "NBOS" main_floppy.img` will create error, assuming we use the same setup. The error I found is fixed by increasing the size of the `main_floppy.img` - change the `count` in `dd` to maybe `100000` so that the file is above 30 MB

Let's go over some important details here:

- The `-i` flag is used to specify the image file we want to access - in this case, `main_floppy.img`
- The drive letter `:` (colon) has a special meaning. It is used to access image files which are directly specified on the command line using the `-i` options. 
- If the target file already exist, overwrite it. Else, create new one

## xxd/hexdump

They are both doing "almost" the same job: display the content of a file in the wanted format. Often they are used to display the content of a file as hex. Be noted that you may see the output of these two commands differ from each other - this is because of your machine endian. `hexdump` without any options will read the file and then use your machine endian to reorder it (see [Does hexdump respect the endianness of its system?](https://unix.stackexchange.com/questions/55770/does-hexdump-respect-the-endianness-of-its-system)) while `xxd` will print out the individual bytes in the file in the order in which they exist (see [Differences between objdump and xxd](https://stackoverflow.com/questions/41227875/differences-between-objdump-and-xxd)). Preferably, just use `xxd`

```bash
xxd hello.py

00000000: 6865 6c6c 6f20 776f 726c 64              hello world
```

As you can see, the command output "clump" two bytes together. `68` is the hex for `h`, `65` is the hex for `e`