MEMORY {
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rw) : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS {
    .isr_vector : 
    {
        *(.isr_vector)
    } > FLASH

    .text : 
    {
        *(.text)
    } > FLASH
    
    .data : 
    {
        _sdata = .;
        *(.data) /* Moves the linker address */
        _edata = .;
    } > RAM AT> FLASH
    
    _sidata = LOADADDR(.data); /* Stores the FLASH address of .data */

    .bss : 
    {
        _sbss = .;
        *(.bss) /* Moves the linker address */
        _ebss = .;
    } > RAM
}