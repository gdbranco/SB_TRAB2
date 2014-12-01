section .data
	log: db "LOG",0ah
	m_size: EQU $-log
	_L2: EQU 4
	__ERROR_MSG: db "Error: Utilizado mais memoria do que alocado",0ah
	__ERROR_MSG_SIZE: EQU $-__ERROR_MSG

section .bss
	_L1: resb _L2

section .text
global _start
_start:
	push _L2
	push _L1
	call __read_string
__loko:

	push _L1
	call __print_string


	mov eax, 1
	mov ebx, 0
	int 0x80

; ------------------  READ -----------------

__read_string:
	enter 0,0
	push ebx
	push ecx
	push edx
	push esi
	xor esi, esi
__read_char:
	mov eax, 3
	mov ebx, 0
	mov ecx, [ebp + 8]
	add ecx, esi
	mov edx, 1
	int 0x80

	inc esi

	mov eax, [ebp + 8]
	cmp byte [eax + esi - 1], 0x0A
	je __end_read
	cmp esi, [ebp + 12]
	je __end_read_full
	jmp __read_char

__end_read:
	mov ecx, [ebp + 8]
	add ecx, esi
	dec si
	mov ax, si
	mov byte [ecx], 0x0A
	jmp __end_read_string

__end_read_full:
	mov ecx, [ebp + 8]
	add ecx, esi
	mov byte [ecx - 1], 0x0a
	mov eax, 4
	mov ebx, 1
	mov ecx, __ERROR_MSG
	mov edx, __ERROR_MSG_SIZE
	int 0x80
	mov ax, 0

__end_read_string:
	pop esi
	pop edx
	pop ecx
	pop ebx
	leave
	ret 8


;---------------- PRINT ----------------------

__print_string:
	enter 0,0
	push eax
	push ebx
	push ecx
	push edx
	push esi
	xor esi,esi
__print_char:
	mov eax, 4
	mov ebx, 1
	mov ecx, [ebp+8]
	add ecx, esi
	mov edx, 1
	int 0x80

	inc esi

	mov eax, [ebp+8]
	cmp byte [eax + esi -1], 0x0a
	je __end_print

	jmp __print_char

__end_print:
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	leave
	ret 4
