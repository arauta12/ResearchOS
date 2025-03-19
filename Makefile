BIN=bin
BOOTLOADER=$(BIN)/boot.bin
OS=$(BIN)/kernel.bin
DISK=$(BIN)/os.img

BOOTLOADER_SRCS := $(wildcard *.asm)
BOOTLOADER_OBJS := $(patsubst %.asm, $(BIN)/%.bin, $(BOOTLOADER_SRCS))

bootdisk: $(BOOTLOADER) $(OS)
	dd if=/dev/zero of=$(DISK) bs=512 count=2048
	dd conv=notrunc if=$(BOOTLOADER) of=$(DISK) bs=512 count=1 seek=0
	dd conv=notrunc if=$(OS) of=$(DISK) bs=512 count=1 seek=1
$(BIN)/%.bin: %.asm
	nasm -f bin $< -o $@
clean:
	rm $(BIN)/*.bin
fclean:
	rm $(BIN)/*