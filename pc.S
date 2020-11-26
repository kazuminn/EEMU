GLOBAL _pc

SECTION .text
_pc:
    jmp far [rdi]
    ret