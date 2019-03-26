BITS 32
start:
        xor [eax+0x12345678],eax
        xor eax,[eax+0x12345678]
;        xor [eax+0x12345678],0x12345678
;        xor [eax+0x12345678],0x12
        xor eax,0x12345678

