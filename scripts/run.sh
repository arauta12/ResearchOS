#!/bin/bash
ARCH=$1
BOOT_FILE=$2
DEBUG=

if [[ "$BOOT_FILE" == "-d" ]]; then
    DEBUG=debug
    BOOT_FILE=$3
fi

if [[ ! -f "$BOOT_FILE" ]]; then
    if [[ -f "cdros.iso" && -f "hdros.img" ]]; then
        echo "Select IMG (1) or ISO (2): "
        read OPT
        while [[ "$OPT" != "1" && "$OPT" != "2" ]]; do
            echo "Invalid option! IMG (1) or ISO (2): "
            read OPT
        done

        if [[ "$OPT" == "1" ]]; then
            BOOT_FILE=cdros.iso
        else
            BOOT_FILE=hdros.img
        fi
        
    elif [[ -f "cdros.iso" ]]; then
        BOOT_FILE=cdros.iso
    elif [[ -f "hdros.img" ]]; then
        BOOT_FILE=hdros.img
    else
        echo "Error: No boot file found!"
        exit 1
    fi
fi

scripts/qemu.sh $ARCH $BOOT_FILE $DEBUG