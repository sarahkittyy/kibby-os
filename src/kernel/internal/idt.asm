global flush_idt
extern idt
flush_idt:
	lidt [idt]
	sti
	ret

%macro nec_ih 1
global isr_%1
isr_%1:
	cli
	push byte 0 ; empty error code
	push byte %1 ; irq id
	jmp isr_stub
%endmacro

%macro ec_ih 1
global isr_%1
isr_%1:
	cli
	push byte %1 ; irq id
	jmp isr_stub
%endmacro

extern interrupt_handler
; stolen from http://www.osdever.net/bkerndev/Docs/isrs.htm
isr_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, interrupt_handler
	call eax ; preserves eip
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa

	add esp, 8
	iret

nec_ih 0
nec_ih 1
nec_ih 2
nec_ih 3
nec_ih 4
nec_ih 5
nec_ih 6
nec_ih 7
ec_ih 8
nec_ih 9
ec_ih 10
ec_ih 11
ec_ih 12
ec_ih 13
ec_ih 14
nec_ih 15
nec_ih 16
ec_ih 17
nec_ih 18
nec_ih 19
nec_ih 20
nec_ih 21
nec_ih 22
nec_ih 23
nec_ih 24
nec_ih 25
nec_ih 26
nec_ih 27
nec_ih 28
nec_ih 29
ec_ih 30
nec_ih 31
; irqs
nec_ih 32
nec_ih 33
nec_ih 34
nec_ih 35
nec_ih 36
nec_ih 37
nec_ih 38
nec_ih 39
nec_ih 40
nec_ih 41
nec_ih 42
nec_ih 43
nec_ih 44
nec_ih 45
nec_ih 46
nec_ih 47
