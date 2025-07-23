INCLUDE_PATH := include
BIN_PATH := bin

C_SRCS := $(shell find . -name *.c)
ASM_SRCS := $(shell find . -name *.S)

ALL_SRCS := $(C_SRCS) $(ASM_SRCS)

# ALL_OBJS := $(wildcard bin/*.o)
# BUILD_OBJS := $(filter-out $(BIN_PATH)/boot.o, $(ALL_OBJS))
OS_IMG := $(BIN_PATH)/os.bin

CFLAGS += -m32 -ffreestanding -nostdlib -z noexecstack -no-pie

export INCLUDE_PATH


$(OS_IMG): $(ALL_SRCS)
	-mkdir -p bin
	cd kernel && $(MAKE)
	cd drivers && $(MAKE)
	gcc $(CFLAGS) -T linker.ld $(BIN_PATH)/*.o -o $(OS_IMG)


.PHONY: iso
iso: $(OS_IMG)
	cp $(OS_IMG) etc/boot/
	@grub-mkrescue -o os.iso etc
