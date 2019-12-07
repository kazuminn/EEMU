#ifndef PIC_H_
#define PIC_H_

#include "Device.h"
#include <queue>

class PIC : public Device {
public:
    void chk_irq();
    std::queue<int> interrupt_queue;
    PIC();
    bool irq[16];
	uint8_t out8(int port);
	void in8(int port, uint8_t data);
};

#endif
