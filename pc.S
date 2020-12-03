GLOBAL _pc

SECTION .text
_pc:
    mov [rsp],rsi
    mov [rsp + 2],rdi
    jmp far [rsp] 
    ret