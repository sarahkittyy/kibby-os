USER_CODE_SEG equ (0x18 | 0x03)
USER_DATA_SEG equ (0x20 | 0x03)

global userexec
extern usermode_fn
userexec:
	cli

	mov ebx, dword [esp + 4]

	mov ax,USER_DATA_SEG
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax ; ss is handled by iret

	mov eax, esp
	push USER_DATA_SEG
	push eax
	pushf

	; set interrupts to re-enable themselves
	pop eax ; get the eflags register
	or eax, 0x200 ; set the if flag
	push eax ; push eflags back onto the stack

	push USER_CODE_SEG
	; push the address of whatever code we want to run in usermode.
	push ebx

	iret
