#!/bin/bash

KRNL_PATH=$1 # REQUIRED
FS_TYPE=$2
IMG_NAME=$3
ROOT_DIR=$4

verify () {
    if [[ -z "$1" ]]; then
        return 0
    else
        return 1
    fi
}

# Arg 1: value to check if empty
# Arg 2: default value
# Arg 3: msg if empty
checkDefault () {
    if [[ -z "$1" ]]; then
        echo $2
    else
        echo $1
    fi

    return 0
}

verify $KRNL_PATH
FS_TYPE=$(checkDefault $FS_TYPE fat32)
IMG_NAME=$(checkDefault $IMG_NAME hdros.img)
ROOT_DIR=$(checkDefault $ROOT_DIR mnt/)

IMG_SIZE_GB=3 # 3GB
KRNL_NAME=$(basename $KRNL_PATH)

# Find available loopback device
LOOP_DEV=$(losetup -f)

# Check if .img file is valid to speed up process
if [ ! -f "$IMG_NAME" ]; then
    dd if=/dev/zero of=$IMG_NAME bs=1G count=$IMG_SIZE_GB
else
    # If size too small, remake it
    CUR_SIZE=$(stat -c %s $IMG_NAME)
    if [ $CUR_SIZE -lt $(($IMG_SIZE_GB * 1024 * 1024)) ]; then
        dd if=/dev/zero of=$IMG_NAME bs=1G count=$IMG_SIZE_GB
    fi
fi

# Use parted utility with a command script to format .img
# Create GPT formatted file (for UEFI systems)
# Creates a EFI System Partition (with FAT32) and 
# A data partition with custom filesystem
parted -s $IMG_NAME -- mklabel msdos \
    mkpart primary $FS_TYPE 2MiB -1MiB \
    set 1 boot on

# Setup loopback device with partitions identified
sudo losetup --partscan $LOOP_DEV $IMG_NAME

case $FS_TYPE in 
    fat32)
        sudo mkfs.fat -F 32 "$LOOP_DEV"p1
    ;;

    fat16)
        sudo mkfs.fat -F 16 "$LOOP_DEV"p1
    ;;

    ext[2-4])
        sudo mkfs.$FS_TYPE "$LOOP_DEV"p1
    ;;

    ntfs)
        sudo mkfs.$FS_TYPE "$LOOP_DEV"p1
    ;;

    *)
        echo "Filesystem not supported. Defaulting to fat32..."
        sudo mkfs.fat -F 32 "$LOOP_DEV"p1
    ;;
esac

# Create and mount the directories for both partitions
mkdir -p $ROOT_DIR
sudo mount "$LOOP_DEV"p1 $ROOT_DIR

# Setup installation for GRUB
sudo mkdir -p $ROOT_DIR/boot/grub

# Install GRUB for x64 EFI system on EFI mountpoint
# TODO: add commands to support Secure boot?
sudo grub-install \
    --target=i386-pc \
    --root-directory=$ROOT_DIR \
    --boot-directory=$ROOT_DIR/boot \
    $LOOP_DEV

# Copy grub configuration and krnl files
sudo cp $KRNL_PATH $ROOT_DIR/boot
sudo scripts/gencfg.sh $ROOT_DIR/boot/grub /boot/$KRNL_NAME
sudo mkdir -p $ROOT_DIR/{home,bin,sys,usr}

# Clean up mountpoints, loopback devices, and mount folders
sudo umount $ROOT_DIR
sudo losetup -d $LOOP_DEV
sudo rm -rf $ROOT_DIR

echo "✅ Finished setup!"
exit 0
