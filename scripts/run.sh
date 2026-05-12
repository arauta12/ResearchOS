#!/bin/bash
ARCH=$1         # REQUIRED
BOOT_DIR=$2     # REQUIRED
DEBUG=$3
BOOT_FILE=

ISO_NAME=$4
IMG_NAME=$5

# Arg1: value, Arg2: default
set_default () {
    if [[ -z "$1" ]]; then
        echo $2
    else
        echo $1
    fi
}

# Arg1: ISO PATH, Arg2: IMG PATH
get_latest_file() {
    ISO_MOD_TIME=$(stat -c "%Y" $1)
    IMG_MOD_TIME=$(stat -c "%Y" $2)

    if [ $ISO_MOD_TIME -ge $IMG_MOD_TIME ]; then
        echo $1
    else
        echo $2
    fi
}

if [[ "$DEBUG" != "debug" ]]; then
    DEBUG=
    ISO_NAME=$3
    IMG_NAME=$4
fi

ISO_NAME=$(set_default $ISO_NAME cdros.iso)
IMG_NAME=$(set_default $IMG_NAME hdros.iso)


if [[ ! -d "$BOOT_DIR" ]]; then
    echo "'$BOOT_DIR' does not exist!"
    exit 1
fi

ISO_PATH=$BOOT_DIR/$ISO_NAME
IMG_PATH=$BOOT_DIR/$IMG_NAME

if [[ -f "$ISO_PATH" && -f "$IMG_PATH" ]]; then
    BOOT_FILE=$(get_latest_file $ISO_PATH $IMG_PATH)
elif [[ -f "$ISO_PATH" ]]; then
    BOOT_FILE=$ISO_PATH
elif [[ -f "$IMG_PATH" ]]; then
    BOOT_FILE=$IMG_PATH
else
    echo "Error: No boot file found!"
    exit 1
fi

scripts/qemu.sh $ARCH $BOOT_FILE $DEBUG