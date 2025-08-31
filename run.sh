#!/bin/bash
VM_OPTION=$1

shift 1
VAR_OPTIONS=$@

if [ "$VM_OPTION" = "vb" ]; then
    # VBoxManage convertfromraw os.img ResearchOS.vdi --format VDI
    # mv ResearchOS.vdi scripts/vbFiles/
    scripts/vb.sh $VAR_OPTIONS
elif [ "$VM_OPTION" = "bochs" ]; then
    scripts/bochs.sh $VAR_OPTIONS
else
    scripts/qemu.sh $VAR_OPTIONS
fi