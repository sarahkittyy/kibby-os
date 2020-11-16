global flush_idt
extern idt
flush_idt:
	lidt [idt]
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
	push byte %1 ; irq id
	jmp isr_stub
%endmacro

extern interrupt_handler
isr_stub:
	pushad

	push ds
	push es
	push fs
	push gs

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, interrupt_handler

	call eax ; preserves eip

	pop eax
	pop gs
	pop fs
	pop es
	pop ds

	popad

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
