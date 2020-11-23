USER_CODE_SEG equ (0x14 | 0x03)
USER_DATA_SEG equ (0x18 | 0x03)

global enter_usermode
enter_usermode:
	mov ax,USER_DATA_SEG
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax ; ss is handled by iret

	mov eax, esp
	push USER_DATA_SEG
	push eax
	pushf
	push USER_CODE_SEG
	; push the address of whatever code we want to run in usermode.
	; push usermode_function

	iret
