ENTRY(start)
OUTPUT(binary)

PHDRS {
    code PT_LOAD FLAGS(0x1 | 0x4);
}

SECTIONS {
    . = 0x2000;
    .text : { *(.text) } :code
    .data : { *(.data) }
    .bss : { *(.bss) }
    .rodata : { *(.rodata) }
}