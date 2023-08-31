# Linux commands

## gzip and gunzip

`gzip` is used to compress file while `gunzip` is used to decompress file (usually files that are compressed by `gzip`). By default, they both compress/decompress the original file, meaning the original file will be replaced by the compress/decompressed file

## dd

See [When and How to Use the dd Command](https://www.baeldung.com/linux/dd-command). Additional documents are [arch linux dd](https://wiki.archlinux.org/title/Dd)

In the venerable Unix command dd, the disk/data duplicator (or, sometimes, disk destroyer) allows us to **copy** raw data from one source to another. It’s not used to copy individual files like cp. Instead, it lets us read from and write to block devices — for example, physical hard drives. We can use this in tasks like

- copying an image file to a USB boot drive
- reading data off of inaccessible or crashed file systems.

The `/dev/zero` is a special file in Unix-like operating systems that provides as many null characters (ASCII NUL, 0x00) as are read from it. One of the typical uses is to provide a character stream for initializing data storage

```bash
# Creating a 1 MiB file, called foobar, filled with null characters
dd if=/dev/zero of=foobar count=1024 bs=1024
# Destroying existing data on a file system partition or drive
dd if=/dev/zero of=/dev/<destination drive or partition>
```

The `if` is the input file and `of` is the output file. Note that the `of` file will be completely replaced with the content of the `if` file

```bash
dd if=/dev/sda of=/dev/sdb
```

This would slowly transfer everything from one disk to another, obliterating everything on the out file disk sdb. Everything there would be gone, replaced by what was on sda.

Note that if `sdb` was a larger disk than `sba`, our filesystem would not resize. It would remain an exact raw binary duplicate, so it would look like the smaller source disk

Notable options:

- `bs`: specify [block size](https://www.ibm.com/docs/en/storage-scale/5.1.8?topic=considerations-block-size), i.e In a file system, a block is the largest contiguous amount of disk space that can be allocated to a file and also the largest amount of data that can be transferred in a single I/O operation. The block size determines the maximum size of a read request or write request that a file system sends to the I/O device driver. The default is 512 bytes
- `count`: specify the number of block to copy from input file to output file. For example, `dd if=/dev/zero of=first.img bs=1G count=10` will copy one gigabyte of zeros ten times to `first.img`

## mkfs

The `mkfs` command makes a new file system on a specified device. The `mkfs` command initializes the volume label, file system label, and startup block. More often that not, we use a file-type specific like `mkfs.fat` or `mkfs.ntfs` (see [What's the difference between mkfs and mkdosfs?](https://askubuntu.com/questions/87024/whats-the-difference-between-mkfs-and-mkdosfs)). For all file system creation tools, see [Arch Linux file system](https://wiki.archlinux.org/title/File_systems). This will focus on `mkfs.fat`

