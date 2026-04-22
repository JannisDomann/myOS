OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_start)

SECTIONS
{
	. = 0x200000;

	.text : {
		*(.entry)
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
		*(COMMON)
	}

	_kernel_end = .;
}
