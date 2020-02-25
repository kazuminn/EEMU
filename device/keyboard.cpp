//
// Created by emu on 2020/02/06.
//
#include "keyboard.h"

keyboard::keyboard(){
}

void keyboard::out8(uint16_t addr, uint8_t data){

}
uint8_t keyboard::in8(uint16_t addr){
    extern unsigned char out_buf;
    //out_buf.pop();
    fprintf(stderr, "jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj");
    switch(addr) {
        case 0x60: return 26;
    }
    return -1;
}

