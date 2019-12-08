#include "interrupt.h"

void Interrupt::exec_interrupt(PIC *pic, Emulator *emu) {
    IntGateDesc idt;
    uint32_t idt_base;
    uint16_t idt_limit, idt_offset;
    idt_offset = pic->INT[pic->interrupt_queue.front()];
    pic->interrupt_queue.pop();

    emu->read_data(&idt, idt_base + idt_offset, sizeof(IntGateDesc));

    emu->EIP = (idt.offset_h << 16) + idt.offset_l;
}
