OBJS 		:= 	entry.o early_print.o main.o io.o
SRC_DIRS 	:=
LINKER		:= 	linker.ld
OBJS 		:= 	$(patsubst %.o,$(BUILD)/%.o,$(OBJS))
MK_FILES	+= 	$(abspath Makefile)

.PHONY: all clean submake verify

all: submake krnl verify

include $(MAKE_INCL)

verify:
	$(Q)echo "CHECKING MULTIBOOT2"
	$(Q)grub-file --is-x86-multiboot2 $(KRNL)

krnl: $(OBJS) $(LINKER)
	$(LD) $(LDFLAGS) -T $(LINKER) $(wildcard $(BUILD)/*.o) -o $(KRNL)

clean:
	$(Q)for dir in $(SRC_DIRS); do \
		$(MAKE) -C $$dir clean --no-print-directory; \
	done
	$(Q)$(RM) $(KRNL) $(DEPS)
