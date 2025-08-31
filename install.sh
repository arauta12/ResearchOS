#!/bin/bash

ROOT_DIR=mnt

DEBUG=$1
shift 1
UPDATE=$1

if [ "$DEBUG" = "debug" ]; then
    make debug
else
    UPDATE=$DEBUG
    make
fi

make bin

scripts/installation/installFAT32.sh $UPDATE mnt os.img krnl.exe > report.log

if (( $? != 0 )); then
    sudo umount $ROOT_DIR
    sudo losetup -d /dev/loop0
    sudo rm -rf $ROOT_DIR
    cat report.log
    rm report.log
    exit 1
else
    rm report.log
    echo "Disk img ready!"
fi