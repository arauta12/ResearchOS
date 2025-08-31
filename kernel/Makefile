DIR_ALL_C_SRCS := $(shell find . -name "*.c")
DIR_ALL_C_ASM := $(shell find . -name "*.S")

C_SRCS := $(wildcard *.c)
C_OBJS := $(patsubst %.c, ../bin/%.o, $(C_SRCS))

ASM_SRCS := $(wildcard *.S)
ASM_OBJS := $(patsubst %.S, ../bin/%.o, $(ASM_SRCS))

ifndef INCLUDE_PATH
	INCLUDE_PATH := include
endif

.PHONY: all
all: $(DIR_ALL_C_SRCS) $(DIR_ALL_C_ASM) $(C_OBJS) $(ASM_OBJS)
	cd boot && $(MAKE)
	cd mm && $(MAKE)

$(C_OBJS): ../bin/%.o : %.c
	$(CC) $(CFLAGS) -c $< -I ../$(INCLUDE_PATH) -o $@

$(ASM_OBJS): ../bin/%.o : %.S
	$(CC) $(CFLAGS) -c $< -I ../$(INCLUDE_PATH) -o $@
