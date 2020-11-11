ENTRY(_start)

SECTIONS
{
	/* 1 MiB is the convential place to start putting sections */
	. = 1M;

	/* mb header has to be early on */
	.text : ALIGN(4096)
	{
		/* multiboot header section first, and then all the .text code following. */
		*(.multiboot)
		*(.text)
	}

	.data : ALIGN(4096)
	{
		*(.data)
	}

	.bss : ALIGN(4096)
	{
		*(COMMON) /* refers to all uninitialized data in all input files */
		*(.bss)
	}
}
