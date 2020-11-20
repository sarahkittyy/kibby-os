extern page_dir

KERNEL_V_ADDR equ 0xC0000000
KERNEL_V_PAGENO equ (KERNEL_V_ADDR >> 22)

global flush_paging
flush_paging:
	; mov the page dir addr to cr3
	mov eax, (page_dir - KERNEL_V_ADDR)
	mov cr3, eax

	ret
