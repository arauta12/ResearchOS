#!/bin/bash
GRUB_CFG_DIR=$1
KRNL_GRUB_PATH=$2

if [[ -z "$KRNL_GRUB_PATH" ]]; then
    echo "Kernel name not provided. Exiting..."
    exit 1
fi

if [[ ! -d "$GRUB_CFG_DIR" ]]; then
    echo "Given grub directory does not exist. Defaulting to iso/boot/grub ..."
    mkdir -p iso/boot/grub
    GRUB_CFG_DIR=iso/boot/grub
fi

cat > $GRUB_CFG_DIR/grub.cfg << EOF
set default=0
set timeout=5

insmod all_video
menuentry "ResearchOS" {
    multiboot2 $KRNL_GRUB_PATH
    boot
}
EOF
