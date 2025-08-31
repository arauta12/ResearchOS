#!/bin/bash
set -e

UPDATE=$1
if [ "$UPDATE" = "u" ]; then
    shift 1
fi

ROOT_DIR=$1
IMG_NAME=$2
KRNL_EXE=$3

# Fresh installation
if [ "$UPDATE" != "u" ]; then
    # dd if=/dev/zero of=$IMG_NAME bs=1024 count=131072
    dd if=/dev/zero of=$IMG_NAME bs=1G count=1
    parted -s $IMG_NAME mklabel msdos \
        mkpart primary fat32 1MiB 100% \
        set 1 boot on \
        print all
    echo "✅ Disk image partitioned with MBR!"
fi

sudo losetup --partscan /dev/loop0 $IMG_NAME
echo "✅ Loop devices setup!"

if [ "$UPDATE" != "u" ]; then
    sudo mkfs.fat -F 32 /dev/loop0p1
fi

mkdir -p $ROOT_DIR/boot/grub
sudo mount /dev/loop0p1 $ROOT_DIR
echo "✅ FAT32 filesystem made & mounted!"

if [ "$UPDATE" != "u" ]; then
    sudo grub-install \
        --root-directory=$ROOT_DIR \
        --boot-directory=$ROOT_DIR/boot \
        --no-floppy \
        --modules="normal part_msdos" \
        /dev/loop0
    echo "✅ Installed grub!"
fi

sudo cp bin/$KRNL_EXE $ROOT_DIR/boot
sudo tee $ROOT_DIR/boot/grub/grub.cfg > /dev/null << EOF
menuentry "ResearchOS" {
    multiboot2 /boot/krnl.exe
    boot
}
EOF
sync
echo "✅ Created grub config!"

sudo umount $ROOT_DIR
sudo losetup -d /dev/loop0
sudo rm -rf $ROOT_DIR

echo "✅ Finished setup!"
