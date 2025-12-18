#!/bin/bash
KRNL_NAME=roskrnl.exe
ISO_NAME=ros.iso
ROOT_DIR=mnt
DATA_DIR=$ROOT_DIR/user
EFI_DIR=$ROOT_DIR/efi

# Currently is the --keep option (might be expanded to add more options)
ARG1=$1

# Find available loopback device
LOOP_DEV=$(losetup -f)

# Check if .iso file was already created to speed up process
if [ ! -f "$ISO_NAME" ]; then
    dd if=/dev/zero of=$ISO_NAME bs=1G count=10
fi

# Use parted utility with a command script to format .iso
# Create GPT formatted file (for UEFI systems)
# Creates a EFI System Partition (with FAT32) and 
# A data partition (with FAT32)
parted -s $ISO_NAME -- mklabel gpt \
    mkpart primary fat32 2MiB 102MiB \
    mkpart primary fat32 102MiB -1MiB \
    set 1 esp on \
    name 1 'ESP' \
    name 2 'Research'
echo "✅ Disk image partitioned"

# Setup loopback device with partitions identified
sudo losetup --partscan $LOOP_DEV $ISO_NAME
echo "✅ Loop devices setup!"

# Both partitions use a FAT32 Filesystem
sudo mkfs.fat -F 32 "$LOOP_DEV"p1
sudo mkfs.fat -F 32 "$LOOP_DEV"p2

# Create and mount the directories for both partitions
mkdir -p $DATA_DIR
mkdir -p $EFI_DIR
sudo mount "$LOOP_DEV"p1 $EFI_DIR
sudo mount "$LOOP_DEV"p2 $DATA_DIR

# Setup installation for GRUB
sudo mkdir -p $EFI_DIR/boot/grub

# Install GRUB for x64 EFI system on EFI mountpoint
# TODO: add commands to support Secure boot?
sudo grub-install \
    --target=x86_64-efi \
    --root-directory=$EFI_DIR \
    --efi-directory=$EFI_DIR \
    --boot-directory=$EFI_DIR/boot \
    $LOOP_DEV

# Copy grub configuration and krnl files
sudo cp build/roskrnl.exe $EFI_DIR/boot
sudo tee $EFI_DIR/boot/grub/grub.cfg > /dev/null << EOF
menuentry "ResearchOS" {
    multiboot2 /boot/roskrnl.exe
}

EOF
sync
sudo mkdir -p $DATA_DIR/home
echo "✅ GRUB installed!"

# Option to examine files after GRUB setup
if [ "$ARG1" == "--keep" ]; then
    echo "✅ Finished setup!"
    exit 0
fi

# Clean up mountpoints, loopback devices, and mount folders
sudo umount $EFI_DIR
sudo umount $DATA_DIR
sudo losetup -d $LOOP_DEV
rm -rf $ROOT_DIR

echo "✅ Finished setup!"
exit 0
