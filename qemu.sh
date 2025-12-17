qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -net none \
    -drive format=raw,file=disk.iso -m 6G -machine q35