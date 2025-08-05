OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    . = 2M;
    .text () : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}