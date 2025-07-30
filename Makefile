INCLUDE_PATH := include
BIN_PATH := bin

C_SRCS := $(shell find . -name *.c)
ASM_SRCS := $(shell find . -name *.S)

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

.PHONY: all debug

all, debug: $(BIN_PATH)/$(OS_IMG)

$(BIN_PATH)/$(OS_IMG): $(ALL_SRCS)
	-mkdir -p $(BIN_PATH)
	cd kernel && $(MAKE)
	cd drivers && $(MAKE)
	$(CC) $(CFLAGS) -T linker.ld $(BIN_PATH)/*.o -o $(BIN_PATH)/$(OS_IMG) -g

.PHONY: iso
iso: $(BIN_PATH)/$(OS_IMG)
	-cp $(BIN_PATH)/os_debug.bin etc/boot
	cp $(BIN_PATH)/$(OS_IMG) etc/boot
	@grub-mkrescue -o $(ISO_NAME) etc

clean:
	$(RM) -r $(BIN_PATH)
