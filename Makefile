BIN=bin
BOOTLOADER=$(BIN)/boot.bin
KERNEL_ST=$(BIN)/kernel.bin
SRC=src
OS=$(BIN)/main.bin
DISK=$(BIN)/os.img

C_SRCS := $(wildcard $(SRC)/*.c)
C_OBJS := $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(C_SRCS))

CFLAGS+=-m32 -ffreestanding -nostdlib -Wall -Wextra

bootdisk: $(BOOTLOADER) $(KERNEL_ST) $(OS)
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK) bs=512 count=1 seek=0
	dd conv=notrunc if=$(KERNEL_ST) of=$(DISK) bs=512 count=1 seek=1
	dd conv=notrunc if=$(OS) of=$(DISK) bs=512 seek=2
$(BIN)/boot.bin: boot.asm
	nasm -f bin $< -o $@
$(BIN)/kernel.bin: kernel.asm
	nasm -f bin $< -o $@
$(OS): $(C_OBJS)
	ld -m elf_i386 -T loader.lds $(C_OBJS) -o $@.elf
	objcopy -O binary $@.elf $@ 
$(BIN)/%.o: $(SRC)/%.c
	gcc $(CFLAGS) -c $< -o $@
clean:
	rm $(BIN)/*
	dd if=/dev/zero of=$(DISK) bs=512 count=2880