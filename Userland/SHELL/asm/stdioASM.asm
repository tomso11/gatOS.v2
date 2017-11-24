
GLOBAL readC
GLOBAL writeC
GLOBAL sys_call
GLOBAL cli

section .text

readC:
	push rbp
	mov rbp, rsp
	mov rdx, rdi
	mov rcx, rsi
	mov rdi, 3
	mov rsi, 0
	int 80h
	mov rsp, rbp
	pop rbp
	ret
writeC:
	push rbp
	mov rbp, rsp
	mov rcx, rsi
	mov rdx, rdi
	mov rdi, 4
	mov rsi, 0
	int 80h
	mov rsp, rbp
	pop rbp
	ret
sys_call:
	push rbp
	mov rbp, rsp
	int 80h
	mov rsp, rbp
	pop rbp
	ret

cli:
	push rbp
	mov rbp, rsp
	sti
	mov rsp, rbp
	pop rbp
	ret