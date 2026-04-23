OUTPUT_FORMAT("elf32-i386")
ENTRY(stage1_start) 

SECTIONS
{
    . = 0x7C00;

    .text : {
        *(.entry)
        *(.text)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment)
        *(.eh_frame)
    }
}
