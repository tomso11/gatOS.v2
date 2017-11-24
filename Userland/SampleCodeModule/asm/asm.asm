GLOBAL cli

section .text

cli:
	push rbp
	mov rbp, rsp
	sti
	mov rsp, rbp
	pop rbp
	ret
