OUTPUT_FORMAT("elf32-i386")
ENTRY(stage2_start)

SECTIONS
{
	. = 0x7E00;

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
}
