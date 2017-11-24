;lock.asm

GLOBAL _testAndSet

; returns 1 if lock acquired, else 0
_testAndSet:
    push rbp
    mov rbp, rsp

    xor edx, edx
    xor eax, eax
    xor ecx, ecx
    xor ebx, ebx

    inc ebx

    lock CMPXCHG8B [rdi]

    jnz .lockNotAcquired

    mov rax, 1

    mov rsp, rbp
    pop rbp
    ret
  
.lockNotAcquired:
    mov rax, 0

    mov rsp, rbp
    pop rbp
    ret