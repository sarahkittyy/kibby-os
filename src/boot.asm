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

; multiboot does not designate a stack pointer
section .bss
align 16
stack_bot:
resb 2 * MiB
stack_top:

; multiboot wants to know where to start! we're startin here at _start
section .text
global _start
_start:
	; set up the stack! esp points to the top of the stack and it grows downwards
	mov esp, stack_top

	; call the kernel_main function
	extern kernel_main
	call kernel_main

	; disable interrupts
	cli
; this *SHOULD* hang forever cuz it waits for an interrupt (which we disabled) but fuck sometimes some weird settings can still raise interrupts so we inf loop anyway
.hang: hlt 
	jmp .hang ; yeet
