OBJS 		:= 	string.o conversion.o
SRC_DIRS 	:=
LINKER		:= 	linker.ld
OBJS 		:= 	$(patsubst %.o,$(BUILD)/%.o,$(OBJS))
MK_FILES	+= 	$(abspath Makefile)

.PHONY: all clean submake

all: submake $(OBJS)

include $(MAKE_INCL)

clean:
	$(Q)for dir in $(SRC_DIRS); do \
		$(MAKE) -C $$dir clean --no-print-directory; \
	done
	$(Q)$(RM) $(DEPS)
