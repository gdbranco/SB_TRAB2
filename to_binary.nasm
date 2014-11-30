section .text
global _start
__read_string:
enter 0,0
push eax
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
;add ecx, 1
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
mov eax, 1
mov ebx, 0
int 0x80
__end_read_string:
pop esi
pop edx
pop ecx
pop ebx
pop eax
leave
ret 8
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


_start:
push 10
push TESTE
call __read_string
push TESTE
call __print_string
mov eax, 3
mov ebx, 0
mov ecx, B
mov edx, 1
int 0x80
sub word [B], 0x30
LOOP: mov ax, [B]
xor dx, dx
div word [DOIS]
mov [QUOT], ax
mov ax, [QUOT]
mul word [DOIS]
mov [VOLTA], ax
mov ax, [B]
sub word ax, [VOLTA]
mov [RESTO], ax
add word [RESTO], 0x30
mov eax, 4
mov ebx, 1
mov ecx, RESTO
mov edx, 1
int 0x80
sub word [RESTO], 0x30
mov ax, [QUOT]
mov [B], ax
cmp ax, 0
jg LOOP
mov eax, 1
mov ebx, 0
int 0x80
section .data
__ERROR_MSG: db "Error: Utilizado mais memoria do que alocado",0ah
__ERROR_MSG_SIZE: EQU $-__ERROR_MSG
DOIS: dw 2
section .bss
B: resw 1
QUOT: resw 1
RESTO: resw 1
VOLTA: resw 1
TESTE resw 10
