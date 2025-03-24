BIN=bin
BOOTLOADER=$(BIN)/boot.bin
KERNEL_ST=$(BIN)/kernel.bin
KERNEL_HELP=$(BIN)/asm_idt.o
SRC=src
OS=$(BIN)/main.bin
DISK=$(BIN)/os.img

C_SRCS := $(wildcard $(SRC)/*.c)
C_OBJS := $(patsubst $(SRC)/%.c, $(BIN)/%.o, $(C_SRCS))

CFLAGS+=-m32 -ffreestanding -nostdlib -Wall -Wextra

.PHONY: clean
.SILENT: bootdisk clean

bootdisk: $(BOOTLOADER) $(KERNEL_ST) $(OS)
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK) bs=512 count=1 seek=0
	dd conv=notrunc if=$(KERNEL_ST) of=$(DISK) bs=512 count=1 seek=1
	dd conv=notrunc if=$(OS) of=$(DISK) bs=512 seek=2
$(BOOTLOADER): boot.asm
	nasm -f bin $< -o $@
$(KERNEL_ST): kernel.asm
	nasm -f bin $< -o $@
$(KERNEL_HELP): asm_idt.asm
	nasm -f elf $< -o $@
$(OS): $(C_OBJS) $(KERNEL_HELP)
	ld -m elf_i386 -T loader.lds $(C_OBJS) $(KERNEL_HELP) -o $@
$(BIN)/%.o: $(SRC)/%.c
	gcc $(CFLAGS) -c $< -o $@ -g
clean:
	rm $(BIN)/*
	dd if=/dev/zero of=$(DISK) bs=512 count=2880