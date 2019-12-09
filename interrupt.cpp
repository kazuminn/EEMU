#include "interrupt.h"

void Interrupt::exec_interrupt(PIC *pic, Emulator *emu) {
    if (emu->eflags.IF == false){
        return;
    }

    IntGateDesc idt;
    uint32_t idt_base;
    uint16_t idt_limit, idt_offset;
    idt_offset = pic->INT[pic->interrupt_queue.front()];
    pic->interrupt_queue.pop();

    emu->read_data(&idt, idt_base + idt_offset * 8, sizeof(IntGateDesc));

    save_regs(emu);
    emu->EIP = (idt.offset_h << 16) + idt.offset_l;
}

void Interrupt::save_regs(Emulator *emu){
    emu->Push32(emu->get_eflags());
    emu->Push32(emu->sreg[1].sreg);
    emu->Push32(emu->EIP);
}
