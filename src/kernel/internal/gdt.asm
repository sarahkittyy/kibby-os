extern kibby_gdt_table

global gdt_flush

gdt_flush:
	lgdt [kibby_gdt_table]
	; enable protected mode by setting the lowest bit of register cr0
	mov eax, cr0
	or al, 1
	mov cr0, eax
	; perform long jump to reload the gdt
	jmp 0x08:.flush2 ; fwoosh
.flush2:
	; fill segment registers with proper segment values
	; (cs was filled)
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
