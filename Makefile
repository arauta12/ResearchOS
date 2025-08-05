INCLUDE_PATH := include
BIN_PATH := bin

C_SRCS := $(shell find . -name *.c)
ASM_SRCS := $(shell find . -name *.S)

OBJS := $(wildcard $(BIN_PATH)/*.o)
BOOT_OBJS := $(BIN_PATH)/boot.o
KRNL_OBJS := $(filter-out $(BOOT_OBJS), $(OBJS))

ALL_SRCS := $(C_SRCS) $(ASM_SRCS)
CFLAGS += -m32 -ffreestanding -nostdlib -z noexecstack -no-pie


ISO_NAME := os.iso


ifeq ($(MAKECMDGOALS), debug)
	OS_IMG := os_debug.bin
	CFLAGS += -g
else
	OS_IMG := os.bin
endif

export INCLUDE_PATH
export CFLAGS

.PHONY: all bin debug iso

all, debug: $(BIN_PATH)/$(OS_IMG)

$(BIN_PATH)/$(OS_IMG): $(ALL_SRCS)
	@echo $(CFLAGS)
	@echo $(OS_IMG)
	-mkdir -p $(BIN_PATH)
	cd kernel && $(MAKE)
	cd drivers && $(MAKE)

bin: $(OBJS)
	$(CC) $(CFLAGS) -T linker.ld $(BOOT_OBJS) $(KRNL_OBJS) -o $(BIN_PATH)/$(OS_IMG)

iso: $(BIN_PATH)/$(OS_IMG)
	-mkdir -p etc/boot/grub
	-cp $(BIN_PATH)/os_debug.bin etc/boot
	cp $(BIN_PATH)/$(OS_IMG) etc/boot
	@grub-mkrescue -o $(ISO_NAME) etc

clean:
	$(RM) -r $(BIN_PATH)
