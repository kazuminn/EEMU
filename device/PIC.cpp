#include "PIC.h"

PIC::PIC() {
    for (size_t i = 0; i < 16 ; i++) {
        IMR[i] = false;

    }
    //init
    for (size_t i = 0; i < 16 ; i++) {
        IRR[i] = false;

    }
    set_int(0x20);
    set_int(0x28);
}

void PIC::out8(uint16_t addr, uint8_t data){
    switch(addr) {
        case 0x20 : //keyboard
            fprintf(stderr, "rrrrrrrrrrrrrrrrrrrrrrrrrr \n");
            IMR[1] = false; IRR[1] = false;
            IMR[12] = false; IRR[12] = false;
            break;
    }
}

void PIC::set_int(uint8_t int_number){
    if(int_number == 0x28){
        for(size_t i = 8; i < 16; i++){
            INT[i] = int_number;
            int_number++;
        }
    }else{
        for(size_t i = 0; i < 8; i++){
            INT[i] = int_number;
            int_number++;
        }
    }
}

extern std::queue<std::pair <int, int>> out_buf;
int buf;
void PIC::chk_irq(Emulator *emu){

    auto q = out_buf.front();
    if (q.second == 1&& emu->eflags.IF == 1){ //keyboard
        IRR[1] = true;
        buf = q.first;
        fprintf(stderr, "1111111111111111111111111 %x \n", q.second);
    } else if (q.second == 12 && emu->eflags.IF == 1){ //mouse
        IRR[12] = true;
        buf = q.first;
        fprintf(stderr, "10  \n");
    }

    for (size_t i = 0; i < 16 ; i++) {
        if (IRR[i] == true){
            fprintf(stderr, "444444444444444444444444444444 \n");
            if (i == 0 && emu->eflags.IF == true) {
            } else {
                fprintf(stderr, "555555555555555555555555555555 \n");
                interrupt_queue.push(i);
                out_buf.pop();
                IRR[i] = false; // completed the interrupt handling
            }
        }
    }
}


uint8_t PIC::in8(uint16_t addr){
	
}


