#!/bin/bash
set -e

DEBUG=$1

if [ "$DEBUG" = "debug" ]; then
    make debug
else
    make
fi

make bin
make iso

if [ "$DEBUG" != "debug" ]; then
    make clean
fi

echo "ISO file finished building."