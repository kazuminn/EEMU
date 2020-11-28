extern _hoge
GLOBAL _pc

SECTION .text
_pc:
    jmp rdi
    ret