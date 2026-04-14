OUTPUT_FORMAT("elf32-i386")
ENTRY(RealMode)

SECTIONS
{
	.text 0x8000 : {
		*(.text)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
}
