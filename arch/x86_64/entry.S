#include <multiboot2.h>
#define KRNL_STACK_SIZE (8*1024)

    .file       "entry.S"
    .section    .multiboot2

    // Multiboot2 Magic Header
    .align      MB2_BYTE_ALIGN
MbHdr:
    .long       MB2_MAGIC
    .long       MB2_ARCH_X86
    .long       (MbHdrEnd - MbHdr)
    .long       -(MB2_MAGIC + MB2_ARCH_X86 + MbHdrEnd - MbHdr)

    // EFI Boot Services
    .align      MB2_BYTE_ALIGN
MbEfiBoot:
    .word       MB2_EFI_BOOT_TYPE
    .word       0
    .long       (MbEfiBootEnd - MbEfiBoot)
MbEfiBootEnd:

    // EFI enter
    .align      MB2_BYTE_ALIGN
MbEfi64:
    .word       MB2_EFI_AMD64_TYPE
    .word       0
    .long       (MbEfi64End - MbEfi64)
    .long       _start
MbEfi64End:

    // Framebuffer
    .align      MB2_BYTE_ALIGN
MbFbTag:
    .word       MB2_FRAMEBUF_TYPE
    .word       0
    .long       (MbFbTagEnd - MbFbTag)
    .long       0
    .long       0
    .long       0
MbFbTagEnd:

    // Ending tag
    .align      MB2_BYTE_ALIGN
MbLastTag:
    .word       MB2_END_TYPE
    .word       0
    .long       8
MbTagEnd:

MbHdrEnd:

    .text
    .extern     _krnl_end
    .extern     kmain
    .globl      _start
    .type       _start,@function
_start:
    cli
    cld
    movl        $_krnl_end, %esp
    addl        $KRNL_STACK_SIZE, %esp
    cmpl        $MB2_BOOT_MAGIC, %eax
    jne         loop    # Hang for now...
loop:
    hlt
    jmp     loop
