#include "interrupt.h"

void Interrupt::exec_interrupt(PIC *pic){
   auto n = pic->INT[pic->interrupt_queue.front()];
   pic->interrupt_queue.pop();
}
