#include "PIC.h"

PIC::PIC() {

}

uint8_t PIC::out8(int port){
    //init
    for (size_t i = 0; i < 16 ; i++) {
        irq[i] = false;
    }
}

void PIC::chk_irq(){
    for (size_t i = 0; i < 16 ; i++) {
        if (irq[i] == true){
            interrupt_queue.push(i);
        }
    }
}


void PIC::in8(int port, uint8_t data){
	
}


