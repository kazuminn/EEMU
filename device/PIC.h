#ifndef PIC_H_
#define PIC_H_

#include "Device.h"
#include "../Emulator.h"
#include <queue>

class PIC : public Device , public PortIO {
public:
    void set_int(uint8_t data);
    void chk_irq(Emulator *emu);
    std::queue<int> interrupt_queue;
    PIC();
    int INT[8];
	void out8(uint16_t addr, uint8_t data);
	void in8(int port, uint8_t data);
};

#endif
