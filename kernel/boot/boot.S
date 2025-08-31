
	.section .multiboot2,"a"

	.set MAGIC,		0xE85250D6
	.set ARCH,		0
	.set HEADER_LEN,(mutliboot2_header_end - mutliboot2_header)
	.set CHECKSUM,	-(MAGIC + ARCH + HEADER_LEN)

	.set MMAP_TAG_TYPE,	6
	.set FLAGS,			0

	.align 	8
mutliboot2_header:
	.long	MAGIC
	.long	ARCH
	.long	HEADER_LEN
	.long	CHECKSUM
end_tag:
	.long	0
	.long 	8
mutliboot2_header_end:


	.text
	.code32
	.globl	_start
	.type	_start,@function
	.extern	kprintf
	.extern	readBootTable
	.extern KRNL_END
	.extern KRNL_END_PHY
	.extern pmmInit
	.extern kvmmInit
	.extern pg_enable
_start:
	cli
	cld
	cmpl	$0x36d76289, %eax
	jne		invalid
	lgdtl	gdt_descr
	movw	$0x10, %ax
	movw	%ax, %ss
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	ljmpl	$0x8,$continue
invalid:
	hlt
continue:
	movl	STACK_START, %esp
	pushl	%ebx
	call	readBootTable
	call	pmmInit
	call	kvmmInit
	/*call	pg_enable*/
	jmp		paging_continue
paging_continue:
	call	kmain

hang:
	jmp hang


	.globl	STACK_START
	.section .rodata
STACK_START:
	.long	(0x800000 + KRNL_END)
kill_message:
	.string	"Invalid grub signature. Aborting..."

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
