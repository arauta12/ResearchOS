nasm -f bin boot.asm -o bin/boot.bin
dd if=bin/boot.bin of=bin/os.img bs=512 count=1
qemu-system-x86_64 -drive format=raw,file=bin/os.img