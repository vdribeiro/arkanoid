CPU 686
BITS 32

%include 'rtc.inc'
%include 'macros.mac'


section .text
global get_time

proc get_time
%arg hora:byte, minuto:byte, segundo:byte
uses eax, ebx, ecx, edx

	; código de leitura dos segundos
	xor		eax,eax
	xor		ecx,ecx
	xor		edx,edx
	mov 	al, RTC_SEC
	out 	RTC_ADD_REG, al
	in 		al, RTC_DATA_REG
	mov		ecx,eax
	shr		ecx,4
	mov 	edx,10
	imul	ecx,edx
	and 	eax,0x0F
	add 	eax,ecx
	mov		ebx,[segundo]
	mov 	[ebx],al

	; código de leitura dos minutos
	xor		eax,eax
	xor		ecx,ecx
	xor		edx,edx
	mov 	al, RTC_MIN
	out 	RTC_ADD_REG, al
	in 		al, RTC_DATA_REG
	mov		ecx,eax
	shr		ecx,4
	mov 	edx,10
	imul	ecx,edx
	and 	eax,0x0F
	add 	eax,ecx
	mov		ebx,[minuto]
	mov 	[ebx],al

	; código de leitura das horas
	xor		eax,eax
	xor		ecx,ecx
	xor		edx,edx
	mov 	al, RTC_HOUR
	out 	RTC_ADD_REG, al
	in 		al, RTC_DATA_REG
	mov		ecx,eax
	shr		ecx,4
	mov 	edx,10
	imul	ecx,edx
	and 	eax,0x0F
	add 	eax,ecx
	mov		ebx,[hora]
	mov 	[ebx],al

	mov 	al, EOI 
	out 	PIC1_CMD, al
	
	mov 	al, EOI 
	out 	PIC2_CMD, al
	
	endproc
END
