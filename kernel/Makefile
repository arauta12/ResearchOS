C_SRCS := $(wildcard *.c)
C_OBJS := $(patsubst %.c, ../bin/%.o, $(C_SRCS))

ASM_SRCS := $(wildcard *.S)
ASM_OBJS := $(patsubst %.S, ../bin/%.o, $(ASM_SRCS))

# CFLAGS += -m32 -ffreestanding -nostdlib

ifndef INCLUDE_PATH
	INCLUDE_PATH := include
endif

.PHONY: all
all: $(C_OBJS) $(ASM_OBJS)

$(C_OBJS): ../bin/%.o : %.c
	$(CC) $(CFLAGS) -c $< -I ../$(INCLUDE_PATH) -o $@

$(ASM_OBJS): ../bin/%.o : %.S
	$(CC) $(CFLAGS) -c $< -I ../$(INCLUDE_PATH) -o $@
