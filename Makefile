# Targeted architecture
ARCH 		:= i386

# All top-level directories to run make on
# NOTE: the arch directory must be last to build
# the final executable
SRC_DIRS 	:= kernel lib arch/$(ARCH)

# Location to build the final kernel executable
KRNL_DIR 	:= arch/$(ARCH)

# Submake info to globally find all necessary directories
SCRIPTS 	:= scripts
BUILD 		:= $(abspath arch/$(ARCH)/build)
DEPS_DIR	:= $(abspath arch/$(ARCH)/deps)
INCLUDE 	:= $(abspath include)
MAKE_INCL 	:= $(abspath Makefile.include)
MK_FILES	:= $(MAKE_INCL)

# Info to make the .iso and .img files
ISO_SHELL 	:= $(KRNL_DIR)/cdimage.sh
ISO_NAME	:= cdros.iso
IMG_NAME	:= hdros.img
IMG_SHELL 	:= $(KRNL_DIR)/hdimage.sh
BOCHS_PATH 	:= /mnt/c/Program\ Files/Bochs-3.0/bochs.exe
BOCHS_FILE	:= scripts/config/i386-bochs.bxrc

# Filesystem to use for the .img main partition
FS			:= ext2

# Kernel name and location
KRNL 		:= roskrnl
KRNL_PATH 	:= $(KRNL_DIR)/$(KRNL)

# Masks out commands used by default
# Replaced with custom printing to reduce wordage
Q=@

# Default build tools
CC=gcc
AS=as
LD=ld

CFLAGS+= \
		-Wno-unused-variable \
		-Wno-unused-function \
		-ffreestanding \
		-Wall \
		-Wextra \
		-fno-pic \
		-g \
		-I $(INCLUDE)

ifeq ($(ARCH),i386)
	CFLAGS+=-m32
endif

LDFLAGS+=-nostdlib -static -z noexecstack

# Submake shared variables
export Q KRNL MAKE_INCL CC AS LD CFLAGS LDFLAGS
export INCLUDE BUILD MK_FILES ISO_NAME IMG_NAME DEPS_DIR

.PHONY: all deps run debug run-dbg clean iso img test bochs

all debug: $(DEPS_DIR) $(BUILD)
	$(Q)for dir in $(SRC_DIRS); do \
		echo "ENTERING $$dir"; \
		$(MAKE) -C $$dir --no-print-directory; \
	done

$(DEPS_DIR):
	$(Q)mkdir $(DEPS_DIR)

$(BUILD):
	$(Q)mkdir $(BUILD)
	$(Q)echo "CREATED BUILD DIR"

clean:
	$(Q)for dir in $(SRC_DIRS); do \
		$(MAKE) -C $$dir clean --no-print-directory; \
	done

	-$(Q)$(RM) -r $(BUILD)/ $(DEPS_DIR)
	@echo "CLEAN DONE"

iso:
	@printf "ISO\t$(ARCH) $(KRNL)\n"
	$(Q)$(ISO_SHELL) $(KRNL_PATH) $(ISO_NAME)

img:
	@printf "IMG\t$(ARCH) $(KRNL)\n"
	$(Q)$(IMG_SHELL) $(KRNL_PATH) $(FS) $(IMG_NAME)

run:
	@printf "QEMU\t$(ARCH)\n"
	$(Q)$(SCRIPTS)/run.sh $(ARCH) $(KRNL_DIR) $(ISO_NAME) $(IMG_NAME)

run-dbg:
	@printf "QEMU-DBG\t$(ARCH)\n"
	$(Q)$(SCRIPTS)/run.sh $(ARCH) $(KRNL_DIR) debug $(ISO_NAME) $(IMG_NAME)

bochs:
	$(Q)$(BOCHS_PATH) -q -f $(BOCHS_FILE)

test: all iso run
