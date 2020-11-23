extern tss

global flush_tss
flush_tss:	
	; entry 0x28 with the last 2 bits set for ring 3
	mov ax, 0x2B
	ltr ax
	ret
