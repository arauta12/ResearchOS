qemu-system-x86_64 \
    -bios /usr/share/ovmf/OVMF.fd \
    -net none \
    -drive format=raw,file=ros.iso \
    -m 6G \
    -machine q35 \
    -S -s