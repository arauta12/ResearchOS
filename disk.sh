#!/bin/bash

IMG_NAME=disk.iso
BOOT_DIR=research
LOOP_DEV=$(losetup -f)

if [ ! -f "$IMG_NAME" ]; then
    dd if=/dev/zero of=$IMG_NAME bs=1G count=6
fi
parted -s $IMG_NAME mklabel gpt \
    mkpart primary fat32 2MiB 52MiB \
    mkpart primary fat32 52MiB 5GiB \
    set 1 esp on

echo "✅ Disk image partitioned"

sudo losetup --partscan $LOOP_DEV $IMG_NAME
echo "✅ Loop devices setup!"

sudo mkfs.fat -F 32 "$LOOP_DEV"p1
mkdir -p $BOOT_DIR
sudo mount "$LOOP_DEV"p1 $BOOT_DIR
cd $BOOT_DIR
sudo mkdir -p efi/boot/
cd ..
sudo cp BOOTX64.EFI $BOOT_DIR/efi/boot/
echo "✅ FAT32 filesystem made & mounted!"

sudo umount $BOOT_DIR
sudo losetup -d $LOOP_DEV
sudo rm -rf $BOOT_DIR
echo "✅ Finished setup!"

exit 0
