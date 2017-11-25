
GLOBAL _change_process
GLOBAL _yield_process

section .text


_change_process:
	mov rsp, rdi
	popState
	iretq


_yield_process:
	int 70h       	
	ret
