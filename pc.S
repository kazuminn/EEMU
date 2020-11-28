GLOBAL _pc

SECTION .text
_pc:
    mov rsp,rdi
    mov [rsp+4],rsi
    jmp far [rsp]
    ret