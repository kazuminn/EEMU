GLOBAL _pc

SECTION .text
_pc:
    mov [rsp],rsi
    mov [rsp + 4],rdi
    jmp far [rsp] 
    ret