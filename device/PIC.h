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
    int INT[50];
    bool IMR[16];
    bool IRR[16];
	void out8(uint16_t addr, uint8_t data);
	uint8_t in8(uint16_t addr);
};

#endif
