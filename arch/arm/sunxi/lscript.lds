/* ENTRY POINT */
ENTRY(_start)

MEMORY
{
	/* SRAM1 */
	SRAM1 (rwx) : ORIGIN = 0x0000030, LENGTH = 0x0000FFD0
}

SECTIONS
{
	. = 0x00000030;
	
	.start    : { *(.start) } > SRAM1
	
	.text     : { *(.text) } > SRAM1
	
	.rodata	  : { *(.rodata) *(.rodata.*) } > SRAM1
	
	.data	  : { *(.data) *(.data.*) } > SRAM1
	
	.bss	  : ALIGN(4)
	{
		_bss_start = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN (4);
		_bss_end = .;
	} > SRAM1
	
	__bootLoader_stack = 0x00010000;
	
	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
	/DISCARD/ : { *(.note*) }
}
