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

section .data
KERNEL_V_ADDR equ 0xC0000000
KERNEL_V_PAGENO equ (KERNEL_V_ADDR >> 22)
align 4096
boot_page_dir:
	dd 0x00000083 ; first 4MB identity mapped
	times (KERNEL_V_PAGENO - 1) dd 0 ; bunch of empty pages
	; kernel v addr entry
	dd 0x00000083
	times (1024 - KERNEL_V_PAGENO - 1) dd 0 ; fill the rest of the table

section .text
global _boot
_boot:
	; time for paging :D
	mov eax, (boot_page_dir - KERNEL_V_ADDR)
	mov cr3, eax

	; enable PSE
	mov eax, cr4
	or eax, 0x00000010
	mov cr4, eax

	; enable paging
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax

	lea ecx, [_start]
	jmp ecx

; multiboot wants to know where to start! we're startin here at _start
_start:
	; invalidate the first 4MB identity mapping
	mov dword [boot_page_dir], 0
	invlpg [0]

	; set up the stack! esp points to the top of the stack and it grows downwards
	mov esp, stack_top

	; call the kernel_main function
	extern kernel_main
	call kernel_main

; this *SHOULD* hang forever cuz it waits for an interrupt (which we disabled) but fuck sometimes some weird settings can still raise interrupts so we inf loop anyway
.hang: hlt 
	jmp .hang ; yeet

; multiboot does not designate a stack pointer
section .bss
align 16
stack_bot:
resb 16 * KiB
stack_top:
