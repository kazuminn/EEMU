BITS 32
start:
	mov eax,0x12345678 
	mov [eax+0x12345678],dword 0x12345678
        xor [eax+0x12345678],eax
        xor eax,[eax+0x12345678]
        xor eax,0x12345678
        xor [eax+0x12345678],dword 0x12345678
        xor [eax+0x12345678],dword 0x12

