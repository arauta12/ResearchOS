#!/bin/bash
KRNL_PATH=$1
ISO_NAME=$2

if [[ -z "$KRNL_PATH" ]]; then
    echo "ERROR: Path to kernel not provided!"
    exit  1
fi

if [[ -z "$ISO_NAME" ]]; then
    echo "No .iso name provided. Defaulting to cdros.iso"
    ISO_NAME=cdros.iso
fi

KRNL_NAME=$(basename "$KRNL_PATH")

if [[ "$KRNL_NAME" != "roskrnl" ]]; then
    echo "Basename failed!"
    exit 1
fi

mkdir -p iso/boot/grub
scripts/gencfg.sh iso/boot/grub /boot/$KRNL_NAME

if [ $? -ne 0 ]; then
    echo "Grub script failed!"
    exit 1
fi

cp $KRNL_PATH iso/boot/
grub-mkrescue -o $ISO_NAME iso/

rm -r iso/