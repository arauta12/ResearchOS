CROSS=x86_64-w64-mingw32-
CC=gcc
BUILD_DIR=build
KRNL_IMG=roskrnl.exe
L_SCRIPT=linker.ld
ISO_FILE=ros.iso
CFLAGS += -ffreestanding -nostdlib -g -mno-red-zone -fPIE

.PHONY: iso qemu clean debug vb

$(BUILD_DIR)/$(KRNL_IMG): boot.o
	$(LD) $(BUILD_DIR)/$^ -T $(L_SCRIPT) -o $@
	
%.o: %.S
	$(CROSS)$(CC) -c $< $(CFLAGS)  -o $(BUILD_DIR)/$@

clean:
	rm -f build/*

iso:
	./scripts/iso.sh

iso-keep:
	./scripts/iso.sh --keep

vb:
	./scripts/vb.sh

run:
	./scripts/qemu.sh

debug:
	./scripts/debug.sh