MAGIC equ 0x1BADB002
MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
CHECKSUM equ -(MAGIC + (MBALIGN | MEMINFO))

KiB equ 1024
MiB equ KiB * 1024

; define the multiboot header
section .multiboot
align 4
	magic dd MAGIC
	flags dd MBALIGN | MEMINFO
	checksum dd CHECKSUM

section .bss
align 16
stack_bot:
resb 16 * KiB
stack_top:

align 4096
KERNEL_V_ADDR equ 0xC0000000
KERNEL_V_PAGENO equ (KERNEL_V_ADDR >> 22)
boot_page_dir:
	resb 0x1000
	;dd (boot_page_table_i - KERNEL_V_ADDR) | 0x00000003 ; first 4MB identity mapped
	;times (KERNEL_V_PAGENO - 1) dd 0 ; bunch of empty pages
	 ;;kernel v addr entry
	;dd (boot_page_table_i - KERNEL_V_ADDR) | 0x00000003
	;times (1024 - KERNEL_V_PAGENO - 1) dd 0 ; fill the rest of the table
boot_page_table_i:
	;; 4MB of identity mapping
	resb 0x1000
;%assign i 0
;%rep 1024
	;dd (i | 0x03)
;%assign i i+1
;%endrep

section .data
; type, base, lim
%macro seg 3
	dw ((%3 >> 12) & 0xFFFF), (%2 & 0xFFFF) ; limit0_15, base0_15
	db ((%2 >> 16) & 0xFF) ; base 16_23
	db (0x90 | %1) ; type with present and S bits already set
	db (0xC0 | ((%3 >> 28) & 0xF)) ; limit16_19, flags
	db ((%2 >> 24) & 0xFF)
%endmacro
; temporary boot gdt
tmp_gdt:
	dq 0 ; null descriptor
	seg (0x08 | 0x02), (0x0), (0xFFFFFFFF) ; code
	seg (0x02), (0x0), (0xFFFFFFFF) ; data
tmp_gdt_ptr:
	dw (tmp_gdt_ptr - tmp_gdt - 1)
	dd tmp_gdt

section .text
global _boot
_boot:
	; load a temporary gdt
	lgdt [tmp_gdt_ptr]

	xor ebx, ebx
	; zero out the tables
.loop:
	mov [boot_page_dir - KERNEL_V_ADDR + ebx], dword 0
	mov [boot_page_table_i - KERNEL_V_ADDR + ebx], dword 0

	inc ebx
	cmp ebx, 1024
	jne .loop
	;;;;;;;;;;;;;;;;;;;;;

	mov eax, dword boot_page_table_i
	sub eax, KERNEL_V_ADDR

	mov ebx, eax
	or ebx, 0x03
	
	mov ecx, dword boot_page_dir
	sub ecx, KERNEL_V_ADDR

	mov [ecx + 0], ebx
	mov [ecx + (KERNEL_V_PAGENO * 4)], ebx

	xor ebx, ebx ; loop var
	mov ecx, 0x1000
	; identity map the identity page table.
.loop2:
	; set the value of eax + offset to offset*0x1000
	mov eax, ebx
	imul ecx
	or eax, 0x03
	mov [boot_page_table_i - KERNEL_V_ADDR + (ebx * 4)], eax
	
	inc ebx
	cmp ebx, 1024
	jne .loop2

	; time for paging :D
	mov eax, (boot_page_dir - KERNEL_V_ADDR)
	mov cr3, eax

	; enable paging
	mov ebx, cr0
	or ebx, 0x80000001
	mov cr0, ebx


	lea ecx, [_start]
	jmp ecx

; multiboot wants to know where to start! we're startin here at _start
_start:
	; set up the stack! esp points to the top of the stack and it grows downwards
	mov esp, stack_top

	; call the kernel_main function
	extern kernel_main
	call kernel_main

; this *SHOULD* hang forever cuz it waits for an interrupt (which we disabled) but fuck sometimes some weird settings can still raise interrupts so we inf loop anyway
.hang: hlt 
	jmp .hang ; yeet
