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
    .code32
    .extern     _krnl_end
    .extern     kmain
    .globl      _start
    .type       _start,@function
_start:
    cli
    cld
    cmpl    $MB2_BOOT_MAGIC, %eax
    jne     loop    # Hang for now...
    lgdt	gdt_descr
	movw	$0x10, %ax
	movw	%ax, %ss
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	ljmp	$0x8,$continue
continue:
    movl        $_krnl_end, %esp
    addl        $KRNL_STACK_SIZE, %esp
    pushl       %ebx
    call        arch_kmain
    addl        $4, %esp
loop:
    hlt
    jmp     loop

	.section .rodata
	/* Describe GDT table */
gdt_start:
	.long	0
	.long	0
cs_32_entry:
	.word	0xffff		/* Segment Limit first 16 (of 20) */
	.word	0			/* Segment Base 1st 16 bits */
	.byte	0			/* Segment Base next 8 bits */
	.byte	0b10011010  /* Access byte */
	.byte	0b11001111	/* Flags & last 4 bits of limit */
	.byte	0			/* Segment Base last 8 bits */
ds_32_entry:
	.word	0xffff
	.word	0
	.byte	0
	.byte	0b10010010
	.byte	0b11001111
	.byte	0
gdt_end:

gdt_descr:
	.word	(gdt_end - gdt_start - 1)
	.long	gdt_start
