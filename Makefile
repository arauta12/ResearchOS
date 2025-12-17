CFLAGS += -mno-red-zone -ffreestanding -nostdlib -Wl,--subsystem,10 -e efi_main -o BOOTX64.EFI

all: efi.c
	x86_64-w64-mingw32-gcc efi.c $(CFLAGS)

iso: BOOTX64.EFI
	./disk.sh

qemu: disk.iso
	./qemu.sh