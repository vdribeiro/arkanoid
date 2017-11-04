CPU 686
BITS 32

include 'kbc.inc'


section	.data

extern _scd
global _handler_teclado


section .text

_handler_teclado:
	push eax	
	;ler do porto 60
	in al, DATA_REG			
	;colocar o scancode na variavel scd
	mov byte[_scd], al
	;sinaliza EOI para o PIC-1
	mov al, EOI 				
	out PIC1_CMD, al
	pop eax						
	iretd
END
