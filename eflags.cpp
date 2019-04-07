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

bool Emulator::chk_parity(uint8_t v){
    bool p = true;

    for(int i=0; i<8; i++)
        p ^= (v>>i) & 1;

    return p;
}
