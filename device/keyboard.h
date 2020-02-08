//
// Created by emu on 2020/02/06.
//
#include "../Emulator.h"

#ifndef EEMU_KEYBOARD_H
#define EEMU_KEYBOARD_H

class keyboard : public PortIO {
public:
    keyboard();
    void out8(uint16_t addr, uint8_t data);
    uint8_t in8(uint16_t addr);
};

#endif //EEMU_KEYBOARD_H
