KERNEL_V_ADDR equ 0xC0000000
KERNEL_V_PAGENO equ (KERNEL_V_ADDR >> 22)

global flush_paging
flush_paging:
	push eax
	push ebp
	
	; mov the page dir addr to cr3
	mov ebp, esp
	mov eax, [ebp + 4]
	sub eax, KERNEL_V_ADDR
	mov cr3, eax

	pop ebp
	pop eax
	ret
