dd if=/dev/zero of=main_floppy.img bs=512 count=2880
mkfs.fat -F 12 -n "NBOS" main_floppy.img
mcopy -i main_floppy.img hello.txt ::jojo.txt
mcopy -i main_floppy.img ::jojo.txt kimi.py
