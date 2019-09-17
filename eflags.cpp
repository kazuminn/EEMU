#include "Emulator.h"

template uint32_t Emulator::update_eflags_add(uint32_t v1, uint32_t v2);
template uint32_t Emulator::update_eflags_add(uint16_t v1, uint32_t v2);
template uint32_t Emulator::update_eflags_add(uint8_t v1, uint32_t v2);
template <class T> uint32_t Emulator::update_eflags_add(T v1, uint32_t v2){
    bool s1, s2, sr;
    uint64_t result;
    uint8_t size;

    v2 = (T)v2;
    result = (uint64_t)v1 + v2;
    size = sizeof(T)*8;

    s1 = v1 >> (size-1);
    s2 = v2 >> (size-1);
    sr = (result >> (size-1)) & 1;

    SetCarry(result >> size);
    SetParity(chk_parity(result & 0xff));
    SetZero(!result);
    SetSign(sr);
    SetOverflow(!(s1^s2) && s1^sr);

    return eflags.reg32;
}

template uint32_t Emulator::update_eflags_sub(uint32_t v1, uint32_t v2);
template uint32_t Emulator::update_eflags_sub(uint16_t v1, uint32_t v2);
template uint32_t Emulator::update_eflags_sub(uint8_t v1, uint32_t v2);
template <class T> uint32_t Emulator::update_eflags_sub(T v1, uint32_t v2){
    bool s1, s2, sr;
    uint64_t result;
    uint8_t size;

    v2 = (T)v2;
    result = (uint64_t)v1 - v2;
    size = sizeof(T)*8;

    s1 = v1 >> (size-1);
    s2 = v2 >> (size-1);
    sr = (result >> (size-1)) & 1;

    SetCarry(result >> size);
    SetParity(chk_parity(result & 0xff));
    SetZero(result == 0);
    SetSign(sr);
    SetOverflow(s1^s2 && s1^sr);

    return eflags.reg32;
}


template uint32_t Emulator::update_eflags_shl(uint32_t v, uint8_t c);
template uint32_t Emulator::update_eflags_shl(uint16_t v, uint8_t c);
template uint32_t Emulator::update_eflags_shl(uint8_t v, uint8_t c);
template <class T> uint32_t Emulator::update_eflags_shl(T v, uint8_t c){
	T result;
	uint8_t size;

	result = v << c;
	size = sizeof(T)*8;

	SetCarry((v >> (size-c)) & 1);
	SetParity(chk_parity(result & 0xff));
	SetZero(!result);
	SetSign((result >> (size-1)) & 1);
	if(c==1)
		SetOverflow(((v >> (size-1)) & 1) ^ ((v >> (size-2)) & 1));

	return eflags.reg32;
}

template uint32_t Emulator::update_eflags_shr(uint32_t v, uint8_t c);
template uint32_t Emulator::update_eflags_shr(uint16_t v, uint8_t c);
template uint32_t Emulator::update_eflags_shr(uint8_t v, uint8_t c);
template <class T> uint32_t Emulator::update_eflags_shr(T v, uint8_t c){
    T result;
    uint8_t size;

    result = v >> c;
    size = sizeof(T)*8;

    SetCarry((v >> (c-1)) & 1);
    SetParity(chk_parity(result & 0xff));
    SetZero(!result);
    SetSign((result >> (size-1)) & 1);
    if(c==1)
        SetOverflow((v >> (size-1)) & 1);

    return eflags.reg32;
}

template uint32_t Emulator::update_eflags_imul(int32_t v1, int32_t v2);
template uint32_t Emulator::update_eflags_imul(int16_t v1, int32_t v2);
template uint32_t Emulator::update_eflags_imul(int8_t v1, int32_t v2);
template <class T> uint32_t Emulator::update_eflags_imul(T v1, int32_t v2){
    int64_t result;
    uint8_t size;

    v2 = (T)v2;
    result = (int64_t)v1 * v2;
    size = sizeof(T)*8;

    SetCarry((result >> size) != -1);
    SetOverflow((result >> size) != -1);

    return eflags.reg32;
}

bool Emulator::chk_parity(uint8_t v){
    bool p = true;

    for(int i=0; i<8; i++)
        p ^= (v>>i) & 1;

    return p;
}
