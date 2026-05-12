#!/bin/bash
ARCH=$1
BOOT_FILE=$2
BOOT_OPT=

if [[ ! -f "$BOOT_FILE" ]]; then
    echo "ERROR: Could not find boot file."
    exit 1
fi

if [[ "$BOOT_FILE" =~ .*.iso ]]; then
    BOOT_OPT="-cdrom $BOOT_FILE"
else
    BOOT_OPT="-drive file=$BOOT_FILE,format=raw"
fi

echo -e "USING\t$BOOT_FILE"

BIOS=
if [[ -z "$ARCH" ]] || [[ "$ARCH" == "x86_64" ]] || [[ "$ARCH" == "x64" ]]; then
    ARCH=x86_64
    BIOS="-bios /usr/share/ovmf/OVMF.fd"
    echo -e "USING\tUEFI"
else
    echo -e "USING\tBIOS"
fi

DEBUG=$3
if [[ "$DEBUG" == "debug" ]]; then
    echo "DEBUGGING"
    DEBUG="-S -s"
fi

qemu-system-$ARCH \
    $BIOS \
    -net none \
    -m 3G \
    -smp 2 \
    -no-reboot \
    -no-shutdown \
    -monitor stdio \
    $BOOT_OPT \
    $DEBUG
