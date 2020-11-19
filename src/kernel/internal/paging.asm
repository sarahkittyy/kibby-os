extern page_dir

global flush_paging
flush_paging:
	; mov the page dir addr to cr3
	mov eax, page_dir
	mov cr3, eax

	; set the PG and PE bits of cr0
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax

	ret
