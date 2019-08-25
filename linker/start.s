section .data
section .text
	global @start
@start:
	call main
	mov eax, 1
	mov ebx, 0
	int 128
