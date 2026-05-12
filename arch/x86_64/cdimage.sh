#!/bin/bash
KRNL_PATH=$1 # REQUIRED
ISO_NAME=$2

KRNL_NAME=$(basename "$KRNL_PATH")
KRNL_DIR=$(dirname "$KRNL_PATH")

if [[ ! -f "$KRNL_PATH" ]]; then
    echo "ERROR: Path to kernel not valid!"
    exit  1
fi

if [[ -z "$ISO_NAME" ]]; then
    ISO_NAME=cdros.iso
fi

mkdir -p iso/boot/grub
scripts/gencfg.sh iso/boot/grub /boot/$KRNL_NAME

if [ $? -ne 0 ]; then
    echo "Grub script failed!"
    exit 1
fi

cp $KRNL_PATH iso/boot/

grub-mkrescue -o "$KRNL_DIR/$ISO_NAME" iso/ > /dev/null 2> /dev/null

if [ $? -eq 1 ]; then
    echo "Error: ISO creating Failed!"
    rm -r iso/
    exit 1
else
    rm -r iso/
    exit 0
fi
