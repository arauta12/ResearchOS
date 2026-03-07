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
    call        kmain
loop:
    hlt
    jmp     loop
