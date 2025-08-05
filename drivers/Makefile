DIR_ALL_C_SRCS := $(shell find . -name "*.c")
DIR_ALL_C_ASM := $(shell find . -name "*.S")

C_SRCS := $(wildcard *.c)
C_OBJS := $(patsubst %.c, ../bin/%.o, $(C_SRCS))

ifndef INCLUDE_PATH
	INCLUDE_PATH := include
endif

export CFLAGS

.PHONY: all
all: $(DIR_ALL_C_SRCS) $(DIR_ALL_C_ASM) $(C_OBJS) $(ASM_OBJS)


$(C_OBJS): ../bin/%.o : %.c
	$(CC) $(CFLAGS) -c $< -I ../$(INCLUDE_PATH) -o $@

$(ASM_OBJS): ../bin/%.o : %.S
	cd ata && $(MAKE)
	$(CC) $(CFLAGS) -c $< -I ../$(INCLUDE_PATH) -o $@
