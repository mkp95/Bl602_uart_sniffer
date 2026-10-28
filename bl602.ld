/* BL602 linker script — memory layout */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 1M
    RAM   (rwx) : ORIGIN = 0x24000000, LENGTH = 128K
}

ENTRY(_start)

SECTIONS
{
    . = ORIGIN(FLASH);

    .text : {
        *(.vectors)
        *(.text*)
        *(.rodata*)
    } > FLASH

    .data : {
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > RAM AT > FLASH

    .bss : {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);
    _end = .;
}
