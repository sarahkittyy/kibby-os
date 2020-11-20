ENTRY(_boot)

SECTIONS
{
	/* 1 MiB is the convential place to start putting sections */
	. = 1M;

	/* mb header has to be early on */
	.multiboot : {
		*(.multiboot)
	}

	/* loading the kernel at 3gb and placing it at 1mb (higher half kernel) */
	. += 0xC0000000;

	.text ALIGN(4096) : AT(ADDR(.text)-0xC0000000)
	{
		*(.text)
	}

	.data ALIGN(4096) : AT(ADDR(.data)-0xC0000000)
	{
		*(.data)
	}

	.rodata ALIGN (4096) : AT(ADDR(.rodata)-0xC0000000)
	{
		*(.rodata*)
	}

	.bss ALIGN(4096) : AT(ADDR(.bss)-0xC0000000)
	{
		*(COMMON) /* refers to all uninitialized data in all input files */
		*(.bss)
	}
}
