SRC_DIRS 	:=
LINKER		:= 	linker.ld
RM_FILES	:= $(KRNL) $(IMG_NAME) $(ISO_NAME) $(DEPS_DIR)/* $(BUILD)/*

.PHONY: all verify

all: $(KRNL)

include $(MAKE_INCL)

$(KRNL): objs $(LINKER)
	@printf "LD\t$(KRNL)\n"
	$(Q)$(LD) $(LDFLAGS) -T $(LINKER) $(wildcard $(BUILD)/*.o) -o $(KRNL)
	$(Q)echo "CHECKING MULTIBOOT2"
	$(Q)grub-file --is-x86-multiboot2 $(KRNL)
