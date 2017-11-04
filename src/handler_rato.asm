CPU 686
BITS 32

section .text

%include 'kbc.inc'

extern _indice
extern _array_bytes

global _handler_rato

_handler_rato:
	push 	eax
	push 	ecx	
	
	xor 	ecx,ecx
	mov		cl,byte [_indice]
	cmp		cl,0x03
	je		indice

leitura:
	in		al,DATA_REG
	
	mov		byte [_array_bytes+ecx],al
	inc		cl
	mov		byte [_indice],cl
	jmp		fim

indice:
	mov		cl,0x00
	mov		[_indice],cl
	jmp		leitura
	
fim:

	mov		al,EOI
	out		PIC1_CMD,al
	out		PIC2_CMD,al
	
	pop 	ecx
	pop 	eax						
	
	iretd
END