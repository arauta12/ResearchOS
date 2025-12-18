#!/bin/bash
ROOT_DIR=mnt
DATA_DIR=$ROOT_DIR/user
EFI_DIR=$ROOT_DIR/efi

sudo umount $EFI_DIR
sudo umount $DATA_DIR
sudo losetup -D
rm -rf $ROOT_DIR