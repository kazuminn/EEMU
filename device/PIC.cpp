#include "PIC.h"

bool IRR[16];
PIC::PIC() {
    //init
    for (size_t i = 0; i < 16 ; i++) {
        IRR[i] = false;

    }
    set_int(0x20);
    set_int(0x28);
}

void PIC::out8(uint16_t addr, uint8_t int_number){
}

void PIC::set_int(uint8_t int_number){
    if(int_number == 0x28){
        for(size_t i = 8; i < 16; i++){
            INT[i] = int_number;
            fprintf(stderr , "INT %x \n", INT[i]);
            int_number++;
        }
    }else{
        for(size_t i = 0; i < 8; i++){
            INT[i] = int_number;
            fprintf(stderr , "INT %x \n", INT[i]);
            int_number++;
        }
    }
}

void PIC::chk_irq(Emulator *emu){
    for (size_t i = 0; i < 16 ; i++) {
        if (IRR[i] == true){
            if (i == 0 && emu->eflags.IF == true) {
            } else {
                interrupt_queue.push(i);
                IRR[i] = false; // completed the interrupt handling
            }
        }
    }
}


uint8_t PIC::in8(uint16_t addr){
	
}


