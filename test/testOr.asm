BITS 32
start:
	mov eax,0x12345678 
	mov [eax+0x12345678],dword 0x12345678
        or [eax+0x12345678],eax
        or eax,[eax+0x12345678]
        or eax,0x12345678
        or [eax+0x12345678],dword 0x12345678
;        or [eax+0x12345678],dword 0x12

