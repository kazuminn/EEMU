//
// Created by emu on 2020/02/06.
//
#include "keyboard.h"
#include "queue"

keyboard::keyboard(){
}

extern std::queue<std::pair <int, int>> out_buf;
void keyboard::out8(uint16_t addr, uint8_t data){
    switch(addr) {
        case 0x60 : out_buf.push(std::make_pair(0xfa, 12));

    }
}

extern std::queue<std::pair <int, int> > out_buf;
extern int buf;
uint8_t keyboard::in8(uint16_t addr){
    //out_buf.pop();
    fprintf(stderr, "77777777777777777777777777 \n");
    switch(addr) {
        case 0x60 : fprintf(stderr, "hogehogehuga %x\n", buf);
                    return buf;

    }
    return -1;
}

