//
// Created by emu on 19/05/22.
#include "Emulator.h"
#include "ModRM.h"

namespace instruction {
    void jmp(Emulator *emu) {
        emu->IP += static_cast<uint16_t>(emu->GetCode8(0));
    }
}

namespace instruction {
    void Init() {
    }
};

