extern _hoge
GLOBAL _pc
GLOBAL _iret

SECTION .text
_pc:
    jmp rdi
    ret
_iret:
    iret