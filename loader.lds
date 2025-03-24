ENTRY(start)
OUTPUT(binary)

PHDRS {
    code PT_LOAD FILEHDR PHDRS FLAGS(0x1 | 0x4) AT(0x2000);
}

SECTIONS {
    .text 0x2000 + SIZEOF_HEADERS : { *(.text) } :code
    .data : { *(.data) }
    .bss : { *(.bss) }
    .rodata : { *(.rodata) }
}