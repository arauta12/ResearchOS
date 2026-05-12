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
ROOT_DIR=$(checkDefault $ROOT_DIR mnt)

IMG_SIZE_GB=2 # 3GB
KRNL_NAME=$(basename $KRNL_PATH)
KRNL_DIR=$(dirname $KRNL_PATH)

IMG_PATH=$KRNL_DIR/$IMG_NAME

# Find available loopback device
LOOP_DEV=$(losetup -f)

# Check if .img file is valid to speed up process
if [ ! -f "$IMG_PATH" ]; then
    dd if=/dev/zero of=$IMG_PATH bs=1G count=$IMG_SIZE_GB
else
    # If size too small, remake it
    CUR_SIZE=$(stat -c %s $IMG_PATH)
    if [ $CUR_SIZE -lt $(($IMG_SIZE_GB * 1024 * 1024)) ]; then
        dd if=/dev/zero of=$IMG_PATH bs=1G count=$IMG_SIZE_GB
    fi
fi

# Use parted utility with a command script to format .img
parted -s $IMG_PATH -- mklabel gpt \
    mkpart primary fat32 2MiB 102MiB \
    mkpart primary $FS_TYPE 102MiB -1MiB \
    name 1 'ESP' \
    name 2 'Research' \
    set 1 esp on

# Setup loopback device with partitions identified
sudo losetup --partscan $LOOP_DEV $IMG_PATH

sudo mkfs.fat -F 32 "$LOOP_DEV"p1
case $FS_TYPE in 
    fat32)
        sudo mkfs.fat -F 32 "$LOOP_DEV"p2
    ;;

    fat16)
        sudo mkfs.fat -F 16 "$LOOP_DEV"p2
    ;;

    ext[2-4])
        sudo mkfs.$FS_TYPE "$LOOP_DEV"p2
    ;;

    ntfs)
        sudo mkfs.$FS_TYPE "$LOOP_DEV"p2
    ;;

    *)
        echo "Filesystem not supported. Defaulting to fat32..."
        sudo mkfs.fat -F 32 "$LOOP_DEV"p2
    ;;
esac

# Create and mount the directories for both partitions
mkdir -p $ROOT_DIR/{data,efi}
sudo mount "$LOOP_DEV"p1 $ROOT_DIR/efi
sudo mount "$LOOP_DEV"p2 $ROOT_DIR/data

# Setup installation for GRUB
sudo mkdir -p $ROOT_DIR/efi/boot/grub

# Install GRUB for i386 system
sudo grub-install \
    --target=x86_64-efi \
    --uefi-secure-boot \
    --removable \
    --efi-directory=$ROOT_DIR/efi \
    --boot-directory=$ROOT_DIR/efi/boot \
    --modules="all_video iso9660 boot part_gpt fat ext2 ntfs efifwsetup" $LOOP_DEV

# Copy grub configuration and krnl files
sudo cp $KRNL_PATH $ROOT_DIR//efi/boot
sudo scripts/gencfg.sh $ROOT_DIR/efi/boot/grub /boot/$KRNL_NAME
sudo mkdir -p $ROOT_DIR/data/{home,bin,sys,usr}

# Clean up mountpoints, loopback devices, and mount folders
sudo umount $ROOT_DIR/data
sudo umount $ROOT_DIR/efi
sudo losetup -d $LOOP_DEV
rm -rf $ROOT_DIR

echo "$IMG_PATH ready!"
exit 0
