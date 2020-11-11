global outb
global inb

outb:
	mov al, [esp + 8] ; byte
	mov dx, [esp + 4] ; addr
	out dx, al ; output byte to addr
	ret

inb:
	mov dx, [esp + 4] ; addr
	in ax, dx
	ret
