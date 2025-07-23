OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    . = 2M;
    .multiboot2 : ALIGN(8) { *(.multiboot2) }
    .text () :  { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}