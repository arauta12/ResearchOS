INCLUDE_PATH := include
BIN_PATH := bin

C_SRCS := $(shell find . -name *.c)
ASM_SRCS := $(shell find . -name *.S)

OBJS := $(wildcard $(BIN_PATH)/*.o)
BOOT_OBJS := $(BIN_PATH)/boot.o
KRNL_OBJS := $(filter-out $(BOOT_OBJS), $(OBJS))

ALL_SRCS := $(C_SRCS) $(ASM_SRCS)
CFLAGS += -m32 -ffreestanding -nostdlib -z noexecstack -no-pie

OS_IMG := krnl.exe

ifeq ($(MAKECMDGOALS), debug)
	CFLAGS += -g -D DEBUG
endif

export INCLUDE_PATH
export CFLAGS

.PHONY: all bin debug clean cleanObjs

all, debug: $(BIN_PATH)/$(OS_IMG)

$(BIN_PATH)/$(OS_IMG): $(ALL_SRCS)
	-mkdir -p $(BIN_PATH)
	cd kernel && $(MAKE)
	cd drivers && $(MAKE)
	cd mm && $(MAKE)
	cd lib && $(MAKE)
	cd fs && $(MAKE)

bin: $(OBJS)
	$(CC) $(CFLAGS) -T linker.ld $(BOOT_OBJS) $(KRNL_OBJS) -o $(BIN_PATH)/$(OS_IMG)

clean:
	$(RM) -r $(BIN_PATH)

cleanObjs:
	$(RM) $(BIN_PATH)/*.o
