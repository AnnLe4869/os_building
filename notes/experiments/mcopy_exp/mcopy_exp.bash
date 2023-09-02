dd if=/dev/zero of=floppy.img bs=512 count=100000
mkfs.fat -F 32 -n "NMOS" floppy.img
mcopy -i floppy.img hello.txt ::jojo.txt
mcopy -i floppy.img ::jojo.txt kimi.py
