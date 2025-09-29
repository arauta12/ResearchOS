#!/bin/bash
set -e

ROOT_DIR=mnt

# DEBUG=$1
# shift 1
OPT1=$1

if [ "$OPT1" = "--help" ]; then
    echo "./install.sh - fresh install"
    echo "./install.sh u - for updating krnl"
    echo "./install.sh du - for updating krnl debug"
    exit 0
fi

make clean
make debug
make bin
mv bin/krnl.exe bin/krnlDg.exe
make cleanObjs

make
make bin


scripts/installation/installFAT32.sh $OPT1 mnt os.img krnl.exe krnlDg.exe > report.log

if (( $? != 0 )); then
    sudo umount $ROOT_DIR
    sudo losetup -d /dev/loop0
    sudo rm -rf $ROOT_DIR
    cat report.log
    exit 1
else
    rm report.log
    echo "Disk img ready!"
fi