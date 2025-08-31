OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS {
    /* Preferred address to load kernel in */
    . = 1M;

    .multiboot2 : { 
        *(.multiboot2)
    }

    /* Create a symbol to reference in code (ie. find exe size) */
    KRNL_ST = .;

    /* Link up the sections and align them on 4K page boundaries */
    .text ALIGN(4K) : {
        *(.text)
    }
        
    .data ALIGN(4K) : {
        *(.eh_frame)
        *(.data)
    }
 
    .rodata ALIGN(4K) : { 
        *(.rodata)
    }
    
    BSS_ALIGN = ALIGN(4K);
    .bss ALIGN(4K) : { 
        *(.bss)
    }
    
    /* Create symbol for 4K aligned end of exe */
    . = ALIGN(4K);
    KRNL_END = .;
}