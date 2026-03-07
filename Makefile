ARCH 		:= i386
SRC_DIRS 	:= lib arch/$(ARCH)
SCRIPTS 	:= scripts
BUILD 		:= $(abspath build)
INCLUDE 	:= $(abspath include)
ISO_SHELL 	:= $(SCRIPTS)/cdimage.sh
IMG_SHELL 	:= $(SCRIPTS)/hdimage.sh
MAKE_INCL 	:= $(abspath Makefile.include)
MK_FILES	:= $(abspath Makefile) $(abspath Makefile.include)
FS			:= ext2

KRNL := roskrnl
KRNL_PATH := arch/$(ARCH)/$(KRNL)

Q=@

CC=gcc
AS=as
LD=ld

CFLAGS+= \
		-m32 \
		-ffreestanding \
		-Wall \
		-Wextra \
		-fno-pic \
		-I $(INCLUDE)
LDFLAGS+=-nostdlib -static -z noexecstack 

ifeq ($(MAKECMDGOALS),debug)
	CFLAGS+=-g
endif

export Q KRNL MAKE_INCL CC AS LD CFLAGS LDFLAGS INCLUDE BUILD MK_FILES

.PHONY: all deps run debug run-dbg clean iso img

all debug: deps $(BUILD)
	$(Q)for dir in $(SRC_DIRS); do \
		echo "ENTERING $$dir"; \
		$(MAKE) -C $$dir --no-print-directory; \
	done

$(BUILD):
	$(Q)mkdir $(BUILD)
	$(Q)echo "CREATED BUILD DIR"

deps:
	$(Q)for dir in $(SRC_DIRS); do \
		$(MAKE) -C $$dir deps --no-print-directory; \
	done

clean:
	$(Q)for dir in $(SRC_DIRS); do \
		$(MAKE) -C $$dir clean --no-print-directory; \
	done

	-$(RM) -r build/

iso:
	$(Q)$(ISO_SHELL) $(KRNL_PATH)

img:
	$(Q)$(IMG_SHELL) $(KRNL_PATH) $(FS)

run:
	$(SCRIPTS)/run.sh $(ARCH)

run-dbg:
	$(SCRIPTS)/run.sh $(ARCH) -d