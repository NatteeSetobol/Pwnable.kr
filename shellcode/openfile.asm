section .data
	file db "test.txt"

section .bss
	var1 resb 255
section .text
global _start
_start:
	mov rdi,0x41414075
	mov rsi,0
	mov rdx, 0666
	mov rax,2
	syscall 

	mov rdi,rax
	mov rax,0
	mov rdx,256
	mov rsi,0x41414155
	syscall

	mov rax,1
	mov rdi,1
	mov rsi,0x41414075 ;0x41414155;0x4141541
	syscall

	mov rax,60
	mov rdi,0
	syscall

