#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <cstdint>
#include <cstring>

#include "Emulator.h"
#include "interrupt.h"
#include "device/PIC.h"
#include "GUI.h"
#include "device/Device.h"
#include "device/keyboard.h"
#include "queue"

#define DEBUG

//#define QUIET

#define INTERNAL_BOXFILL
//#define TEST_VRAM
#define HARIBOTE_UI

using namespace std;

Emulator	*emu;
PIC	*pic;
keyboard *kb;
Interrupt	*inter;
GUI		*gui;
Display		*disp;


int main(int argc, char **argv){
	
	//TODO parse args

	emu = new Emulator();
    pic = new PIC();
    kb = new keyboard();

    inter = new Interrupt();
	cout<<"emulator created."<<endl;
	
	disp = new Display(emu->memory + VRAM_ADDR);
	gui = new GUI(disp);


//	getchar();

	//iplとasmheadの処理
    emu->LoadBinary(argv[1], 0x100000, 1440 * 1024);
	std::memcpy(&emu->memory[0x280000], &emu->memory[0x104390], 512 * 1024);

	uint32_t source;
	uint32_t dest;
	uint32_t interval;
	std::memcpy(&source, &emu->memory[0x280014], 4);
	std::memcpy(&dest, &emu->memory[0x28000c], 4 ); source += 0x280000;
	std::memcpy(&interval, &emu->memory[0x280010], 4);

	std::memcpy(&emu->memory[dest], &emu->memory[source], interval);


	emu->EIP = 0x1b;
	emu->ESP = dest;


	//set vmode scrnx scrny vram
	emu->memory[0xff2] = 8;
	emu->SetMemory16(0xff4, 320);
    emu->SetMemory16(0xff6, 200);
	emu->SetMemory32(0xff8, 0xa0000);
//	emu->memory[0xff8] = 0xa0;

	gui->OpenWindow();
	
//	emu->ESP = 0xffff;
//	emu->DumpRegisters(32);
	//emulation
	emu->set_portio(0x20, 2, pic);
    emu->set_portio(0x60, 1, kb);
    emu->set_portio(0x60, 12, kb);
	for(size_t i = 0; true; i++){
        emu->AX = emu->EAX;
	    emu->AL = emu->EAX;
        emu->AH = emu->EAX;
        emu->DX = emu->EDX;
        emu->DL = emu->EDX;
        emu->DH = emu->EDX;
        emu->CX = emu->ECX;
        emu->CL = emu->ECX;
        emu->CH = emu->ECX;

        //like irq hardware polling
	    pic->chk_irq(emu);

	    //exec INT xx instruction
        inter->exec_interrupt(pic, emu);

		emu->instr.prefix = emu->parse_prefix(emu);

		emu->instr.opcode	= emu->memory[emu->EIP + emu->sgregs[1].base];
		instruction_func_t* func;

		//two byte opecode
		switch(emu->instr.opcode) {
			case 0x0f:
				emu->EIP++;
				emu->instr.opcode = (emu->instr.opcode << 8) + (uint8_t)emu->GetSignCode8(0);
		}

#ifndef QUIET
		cout<<"emu: ";
		cout<<"EIP = "<<hex<<showbase<<emu->EIP<<", ";
		cout<<"Code = "<<(uint32_t)emu->instr.opcode<<endl;
#endif

		if(emu->instr.prefix) {
			func = instructions32[emu->instr.opcode];
		}else {
			func = instructions16[emu->instr.opcode];
		}
		if(func == NULL){
			cout<<"命令("<<showbase<<(int)emu->instr.opcode<<")は実装されていません。"<<endl;
			break;
		}

		//execute
		func(emu);

		//if(i >= 500000){
		//	emu->EIP = 0;
		//	printVram(emu->memory + VRAM_ADDR);
		//}
		if(emu->EIP == 0){
			cout<<"EIP = 0になったので終了"<<endl;
			break;
		}
		
		if(emu->EIP > emu->GetMemSize()){
			cout<<"out of memory."<<endl;
			break;
		}
		//printf("i %d \n", (int)i);

	}
	
	emu->DumpRegisters(32);
	emu->DumpMemory("memdump.bin");
	
	delete emu;
	cout<<"emulator deleted."<<endl;
	
	delete gui;
	delete disp;
	return 0;
}
