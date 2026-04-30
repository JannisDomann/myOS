#include "memory_layout.h"

OUTPUT_FORMAT("elf64-x86-64")
ENTRY(kernel_start)

SECTIONS
{
    . = KERNEL_VIRT_BASE + KERNEL_PHYS_MIN;

	.text : AT(ADDR(.text) - KERNEL_VIRT_BASE)
    {
        _code = .;
		*(.entry)
        *(.text)
        . = ALIGN(4096);
    }

	.rodata : AT(ADDR(.rodata) - KERNEL_VIRT_BASE)
    {
        _rodata = .;
        *(.rodata)
        . = ALIGN(4096);
    }

	.data : AT(ADDR(.data) - KERNEL_VIRT_BASE)
    {
        _data = .;
        *(.data)
        . = ALIGN(4096);
    }

	.bss : AT(ADDR(.bss) - KERNEL_VIRT_BASE)
    {
        _bss = .;
        *(.bss)
        *(COMMON)
        . = ALIGN(4096);
    }

	_kernel_end = .;
}
