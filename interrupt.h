#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "device/PIC.h"

class Interrupt {
private:
public:
    void exec_interrupt(PIC *pic, Emulator *emu);
    void save_regs(Emulator *emu);
};

#endif

