//
// Created by emu on 2020/02/06.
//
#include "keyboard.h"
#include "queue"

keyboard::keyboard(){
}

void keyboard::out8(uint16_t addr, uint8_t data){

}
uint8_t keyboard::in8(uint16_t addr){
    extern std::queue<int> out_buf;
    //out_buf.pop();
    switch(addr) {
        case 0x60 : auto q = out_buf.front();
                    fprintf(stderr, "hogehogehuga %x\n", q);
                    out_buf.pop();
                    return q;

    }
    return -1;
}

